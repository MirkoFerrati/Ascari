#include "agent_router.h"
#include <lemon/dijkstra.h>
#include <lemon/path.h>
#include <lemon/random.h>
#include <time.h>
#include <agent.h>
#include <lemon/adaptors.h>
#include <cassert>
#include "typedefs.h"
#include <lemon/smart_graph.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/dijkstra.h>
#include <lemon/concepts/maps.h>
#include <sstream>
#include <iomanip>
#include<lemon/graph_to_eps.h>
#include "debug_constants.h"
#include "graph_creator.h"



//TODO: probabilmente non serve mandarsi anche la lista dei nodi bloccati, basta la lista degli archi con questa configurazione
//TODO: l'effetto sarebbe una riduzione dei dati da mandarsi e moltissimi cicli in meno per creare e parsare la mappa filtrante
//TODO: oppure ancora più facile mandarsi solo la lista dei nodi bloccati, tanto poi blocco tutti gli archi entranti...e gli uscenti?
lemon::Random agent_router::generatorRandom;

using namespace std;

agent_router::agent_router(std::vector< int > tarlist, std::map< transition, bool >& events,
                           const std::map<std::string, transition>& events_to_index, string identifier,
                           simulation_time& time,std::string graphName
                          )
        :targets(tarlist),events(events),
        events_to_index(events_to_index),
        length(graph),coord_x(graph),coord_y(graph),identifier(identifier),communicator(_mutex,&info,_io_service),time(time)
{
    using namespace lemon;
    Graph_creator c(graph,length,coord_x,coord_y);
    graph_node_size=c.createGraph(MAXFLOORS,graphName);
    if (!graph_node_size)
        ERR("attenzione, impossibile creare il grafo",0);
    tarc=0;
    int id=targets[tarc];
    tarc++;
    source=graph.nodeFromId(id);
    id=targets[tarc];
    tarc++;
    target=graph.nodeFromId(id);
    isNegotiating=true;
    next=source;
    xtarget=(coord_x)[next];
    ytarget=(coord_y)[next];
    info[identifier].timestamp=0;
    setTargetStop(false);
    communicator.startReceive();
    speed=5;
    last_time_updated=time;
    stop=false;
    next_target_reachable=false;
    isWaiting=0;
}




void agent_router::run_plugin()
{
    if (stop)
    {
        setTargetStop(true);
        return;
    }

    lemon::SmartDigraph::ArcMap<bool> useArc(graph,true);
    if (checkIfTargetReached())
    {
        if (setNextTarget())
        {
			isNegotiating=true;
            next_target_reachable=findPath(useArc);
			isWaiting=0;
        }
    }
    else
    {
		isWaiting++;
		_io_service.poll();
		_io_service.reset();
		merge_informations_collided(useArc);
		
        if (isWaiting<10) //Sto ancora negoziando
        {
			next_target_reachable=findPath(useArc);
        }
        else if (isWaiting==10) //finito di negoziare, o parto oppure salto il turno
        {
			if (findPath(useArc))
			{
				next_target_reachable=true;
			}
			else
			{
				prepare_stop_packet();
				next_target_reachable=false;
				cout<<"ho esaurito il tempo per la negoziazione per il target "<<graph.id(target)<<",mi fermo nel nodo"<<graph.id(source)%graph_node_size
				<<"e prenoto i nodi "<<graph.id(source)%graph_node_size+graph_node_size<<graph.id(source)%graph_node_size+2*graph_node_size<<endl;
				isWaiting=19;
			}
			isNegotiating=false; //non negozio il nodo dove sono bloccato, non voglio incidenti da dietro
			last_time_updated=time;
			copy_info_packet();
			communicator.send();
        }
        else if (isWaiting<15)
		{
			if (findPath(useArc))
			{
				next_target_reachable=true;
			}
			else
			{
				next_target_reachable=false;
				prepare_stop_packet();
				cout<<"ero contento di poter arrivare a "<<graph.id(target)<<",ma ho ricevuto qualcosa che mi fa fermare in "<<graph.id(source)%graph_node_size<<endl;
			}
			isNegotiating=false; //non negozio il nodo dove sono bloccato, non voglio incidenti da dietro
			last_time_updated=time;
			copy_info_packet();
			communicator.send();
		}
        else //o sono partito, e quindi target è reachable, oppure sono fermo, e allora salto il turno ma continuo a controllare il percorso
		{
			if (!next_target_reachable)
			{
				if (time-trunc(trunc(time)/TIME_SLOT_FOR_3DGRAPH)*TIME_SLOT_FOR_3DGRAPH<0.2)
				{
					isNegotiating=true;
					next_target_reachable=findPath(useArc);
					isWaiting=0;
				}
			}	
		}
    }
    setTargetStop(!next_target_reachable);
	simulation_time delta= next_time+0.3-time;
    double floor=graph.id(next)/graph_node_size;
    if (floor<0.000001) //floor==0 nei double
        speed=0;
    else
    {
        double length = distance_to_target.value();
        cout<<"calcolo velocità. time="<<time<<" next_time="<<next_time<<" piani="<<floor<<" lunghezza="<<length;
        if (delta<=0)
			speed=0;
		else
			speed=min(1000.0,max((length)/(delta),0.0));
		cout<<" velocità="<<speed<<endl;
    }
}

void agent_router::copy_info_packet()
{
	_mutex.lock();
	graph_informations& tmp = info.at(identifier);
	tmp.id=identifier;
	tmp.isNegotiating=isNegotiating;
	tmp.lockedNode=node_id;
	tmp.timestamp=last_time_updated;
	_mutex.unlock();
}



void agent_router::prepare_info_packet()
{
    using namespace lemon;
    node_id.clear();
    for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
    {
        //Il nodo al piano zero (quello iniziale ) e quello al piano finale non vanno inclusi
        if (graph.id(i)/graph_node_size>FLOORS_SENT || graph.id(i)<graph_node_size)
            continue;
        node_id.push_back(graph.id(i));
    }
   copy_info_packet();
}

bool agent_router::merge_informations_collided(lemon::SmartDigraph::ArcMap<bool>& useArc)
{
    using namespace lemon;
    bool collision=false;
    _mutex.lock();
    for (graph_packet::const_iterator it=info.begin();it!=info.end();it++)
    {
        if (it->first.compare(identifier)==0) continue; //Ignoro me stesso
        bool isNegotiable = (*it).second.isNegotiating;
        int age=round((round(time*1000.0)-round((*it).second.timestamp*1000.0))/1000.0/TIME_SLOT_FOR_3DGRAPH);
        if ((isNegotiable)&&(it->second.id.compare(identifier)>0)) continue; //ignoro le prenotazioni di livello più basso negoziabili
        //TODO: qui sto ignorando gli archi della lista delle prenotazioni che finiscono in un nodo all'ultimo piano
        for (vector<int>::const_iterator itt=(*it).second.lockedNode.begin();itt!=(*it).second.lockedNode.end();itt++)
        {
            int id=(*itt)-age*graph_node_size;
            if (id<graph_node_size) //Se il nodo è finito nel passato oppure al piano terra lo ignoro
                continue;
            //Calcolo le collisioni
            for (unsigned int i=0;i<node_id.size();i++)
            {
                if (node_id[i]==id)
                {
                    collision=true;
                    break;
                }
            }
            //In ogni caso filtro gli archi, mi serviranno per calcolare un nuovo percorso senza collisioni
            for (SmartDigraph::InArcIt arc(graph,graph.nodeFromId(id));arc!=INVALID;++arc)
            {
                useArc[arc]=false;
            }
        }
    }
    _mutex.unlock();
    return collision;
}

void agent_router::prepare_stop_packet()
{
	node_id.clear();
	for (unsigned int i=0;i<MAXFLOORS;i++)//prenoto qualche nodo sopra di me per sicurezza
            node_id.push_back(graph.id(source)%graph_node_size+i*graph_node_size);
	isNegotiating=false; //non negozio il nodo dove sono bloccato, non voglio incidenti da dietro
	last_time_updated=time;
}


bool agent_router::findPath(lemon::SmartDigraph::ArcMap<bool>& useArc)
{
    using namespace lemon;
    if (target==INVALID) return false;
    bool reached;
    merge_informations_collided(useArc);
    reached= dijkstra(filterArcs(graph,useArc),length).path(p).dist(d).run(source,target);
    if (d>MAX_LENGTH)
        reached=false;
    if (!reached)
    {
		prepare_stop_packet();
		cout<<"impossibile trovare un percorso valido per il nodo "<<graph.id(target)<<",distanza minima: "<<d<<endl;
		copy_info_packet();
        communicator.send();
        return false;
    }
    else
	{
    last_time_updated=time;
	prepare_info_packet();
	communicator.send();
    next=graph.target(p.front());
    xtarget=(coord_x)[next];
    ytarget=(coord_y)[next];
	double floor=graph.id(next)/graph_node_size;
	next_time= trunc(trunc(time+TIME_SLOT_FOR_3DGRAPH/3.0)/TIME_SLOT_FOR_3DGRAPH)*TIME_SLOT_FOR_3DGRAPH+TIME_SLOT_FOR_3DGRAPH*floor;//TODO bruttissimo
    std::cout<<" percorso calcolato:";
    for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
        std::cout<<graph.id(i)<<">>";
    std::cout<<" next_time="<<next_time<<" fine"<< std::endl;
    return true;
	}
}



bool agent_router::setNextTarget()
{

    if (graph.id(next)%graph_node_size==graph.id(target))
    {
        source=target;
        if (tarc==targets.size())
        {
            cout<<" TARGET FINALE raggiunto, mi fermo"<<endl;
            stop=true;
            return false;
        }
        int id=targets[tarc];
        tarc++;
        target=graph.nodeFromId(id);
        cout<<" TARGET raggiunto, nuovo target: "<<id<<" "<<(coord_x)[target]<<" "<<(coord_y)[target]<<endl;
    }
    else
    {
        source=graph.nodeFromId(graph.id(next)%graph_node_size);
    }
    return true;
}

