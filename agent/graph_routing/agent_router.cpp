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

agent_router::agent_router(std::vector< int > tarlist, std::map< transition, events::value >& events,
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
    if (targets.size()<2)
		ERR("attenzione, la lista dei target e' troppo corta",0);
    source=graph.nodeFromId(targets[0]);
    target=graph.nodeFromId(targets[1]);
	tarc=2;
    isNegotiating=true;
    next=source;
    xtarget=coord_x[next];
    ytarget=coord_y[next];
    info[identifier].timestamp=0;
//    setTargetStop(false);
    communicator.startReceive();
    last_time_updated=time;
    stop=next_target_reachable=false;
    handshakeCounter=0;
}

bool agent_router::isTimeToCheckForPathFree()
{
    return time-trunc(trunc(time)/TIME_SLOT_FOR_3DGRAPH)*TIME_SLOT_FOR_3DGRAPH<=0.01;
}

void agent_router::run_plugin()
{
    if (stop)
    {
        setTargetStop(true);
        return;
    }

    lemon::SmartDigraph::ArcMap<bool> useArc(graph,true);
	_io_service.poll();
	_io_service.reset();
	
	
	
	if (target_reached())
    {
        setNextTarget();
        isNegotiating=true;
        handshakeCounter=0;
    }

    handshakeCounter++;

    if (handshakeCounter<15)
	{
		if (detect_collision(useArc))
		{
		  cout<<time<<" rilevata una collisione"<<endl;
		}
		
		next_target_reachable=findPath(useArc);
	}
    if (!next_target_reachable)
    {
	prepare_stop_packet();
        if (handshakeCounter<10) //Sto ancora negoziando
        {
            cout<<time<<": impossibile trovare un percorso valido per il nodo "<<graph.id(target)<<",distanza minima: "<<d<<endl;
        }
        if (handshakeCounter==10) //finito di negoziare, o parto oppure salto il turno
        {
            cout<<time<<": ho esaurito il tempo per la negoziazione per il target "<<graph.id(target)<<",mi fermo nel nodo"<<graph.id(source)%graph_node_size
                <<"e prenoto i nodi "<<graph.id(source)%graph_node_size+graph_node_size<<graph.id(source)%graph_node_size+2*graph_node_size<<endl;
            handshakeCounter=15;
        }
        if (handshakeCounter>10 && handshakeCounter<15)//Ebbene può succedere, leggere il cout per capire quando
        {
            cout<<time<<": ero contento di poter arrivare a "<<graph.id(target)<<",ma ho ricevuto qualcosa che mi fa fermare in "<<graph.id(source)%graph_node_size<<endl;
        }
    }
    if (handshakeCounter>=10)
	{
		isNegotiating=false;
	}
	if (handshakeCounter<=15)
	{
		last_time_updated=time;
		priority=round(distance_to_target.value()/100.0);
		update_lock_packet();
		communicator.send();
	}
	if (handshakeCounter>15 && isTimeToCheckForPathFree())
	{
		isNegotiating=true;
		handshakeCounter=0;
	}
	
    setTargetStop(!next_target_reachable);
    simulation_time delta= next_time+0.2-time;
    double floor=graph.id(next)/graph_node_size;
    if (floor<0.000001) //floor==0 nei double
        speed=0;
    else
    {
        double length = distance_to_target.value();
        //cout<<time<<": calcolo velocità "<<" next_time="<<next_time<<" piani="<<floor<<" lunghezza="<<length;
        if (delta<=0)
            speed=0;
        else
            speed=min(1000.0,max((length)/(delta),0.0));
        //cout<<" velocità="<<speed<<endl;
    }
}

void agent_router::update_lock_packet()
{
    _mutex.lock();
    graph_informations& tmp = info.at(identifier);
    tmp.id=identifier;
    tmp.priority=priority;
    tmp.isNegotiating=isNegotiating;
    tmp.lockedNode=node_id;
    tmp.timestamp=last_time_updated;
    _mutex.unlock();
}

void agent_router::prepare_stop_packet()
{
    node_id.clear();
    for (unsigned int i=0;i<MAXFLOORS;i++)//prenoto qualche nodo sopra di me per sicurezza
        node_id.push_back(graph.id(source)%graph_node_size+i*graph_node_size);
    isNegotiating=false; //non negozio il nodo dove sono bloccato, non voglio incidenti da dietro
    last_time_updated=time;
}

void agent_router::prepare_move_packet()
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
    last_time_updated=time;
}

bool agent_router::check_for_overtaking(graph_packet::const_iterator &it,int age,lemon::SmartDigraph::ArcMap<bool>& useArc)
{
	using namespace lemon;
	//Qui controllo per evitare sorpassi, spero sia giusto
	bool collision=false;
	for (unsigned int j=1;j<(*it).second.lockedNode.size();j++)
	{
		int id=(*it).second.lockedNode[j-1]-age*graph_node_size;
		int id1=(*it).second.lockedNode[j]-age*graph_node_size;
		
		for (unsigned int i=1;i<node_id.size();i++)
		{
			if ((node_id[i-1]-id)%graph_node_size==0 &&
				(node_id[i]-id1)%graph_node_size==0 &&
				node_id[i-1]>=id &&
				node_id[i]<=id1)
			{
				collision=true;
				std::cout<<time<<": sto rischiando di sorpassare l'agente "<<(*it).second.id<<" tra il nodo "<<id<<" e "<<id1<<"passando da "<<node_id[i-1]<<" e "<<node_id[i]<<"\n";//<<std::endl;
				int tempId=id1;
				while (tempId>0)
				{
					for (SmartDigraph::InArcIt arc(graph,graph.nodeFromId(tempId));arc!=INVALID;++arc)
					{
						if ((graph.id(graph.source(arc))-id)%graph_node_size==0)
						{
							useArc[arc]=false;
							std::cout<<" disattivo l'arco tra il nodo"<<graph.id(graph.source(arc))<<" e "<<graph.id(graph.target(arc))<<"\n";
						}
					}
					
					//break;
					tempId-=graph_node_size;
				}
			}
		}
	}
	return collision;
}

bool agent_router::detect_collision(lemon::SmartDigraph::ArcMap<bool>& useArc)
{
    using namespace lemon;
    bool collision=false;
    _mutex.lock();
    for (graph_packet::const_iterator it=info.begin();it!=info.end();++it)
    {
        if (it->first.compare(identifier)==0) continue; //Ignoro me stesso
        bool isNegotiable = (*it).second.isNegotiating;
        int age=round((round(time*1000.0)-round((*it).second.timestamp*1000.0))/1000.0/TIME_SLOT_FOR_3DGRAPH);
		//collision=check_for_overtaking(it,age,useArc);
		//if ((isNegotiable)&&(it->second.id.compare(identifier)>0)) continue; //ignoro le prenotazioni di livello più basso negoziabili
	if ((isNegotiable)&&(it->second.priority>priority)) continue; //se sono piu' lontani vinco io
	if ((isNegotiable)&&(it->second.priority==priority)&&(it->second.id.compare(identifier)>0)) continue; //se sono pari vince l'id piu' basso
	//qui sto ignorando gli archi della lista delle prenotazioni che finiscono in un nodo all'ultimo piano
        for (vector<int>::const_iterator itt=(*it).second.lockedNode.begin();itt!=(*it).second.lockedNode.end();++itt)
        {
            int id=(*itt)-age*graph_node_size;
            if (id<graph_node_size) //Se il nodo e' finito nel passato oppure al piano terra lo ignoro
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




bool agent_router::findPath(lemon::SmartDigraph::ArcMap<bool>& useArc)
{
    using namespace lemon;
    if (target==INVALID) return false;
    bool reached;
    //detect_collision(useArc);
    reached= dijkstra(filterArcs(graph,useArc),length).path(p).dist(d).run(source,target);
    if (d>MAX_LENGTH)
        reached=false;
    if (!reached)
        return false;
    prepare_move_packet();
    update_lock_packet();
    communicator.send();
    next=graph.target(p.front());
    xtarget=(coord_x)[next];
    ytarget=(coord_y)[next];
    double floor=graph.id(next)/graph_node_size;
    next_time= trunc(trunc(time+TIME_SLOT_FOR_3DGRAPH/3.0)/TIME_SLOT_FOR_3DGRAPH)*TIME_SLOT_FOR_3DGRAPH+TIME_SLOT_FOR_3DGRAPH*floor;//TODO bruttissimo
    std::cout<<time<<": percorso calcolato:";
    for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
        std::cout<<graph.id(i)<<"("<<graph.id(i)%graph_node_size<<")"<<">>";
    std::cout<<" next_time="<<next_time<<" fine"<< std::endl;
    return true;
}



bool agent_router::setNextTarget()
{
    source=graph.nodeFromId(graph.id(next)%graph_node_size);
    if (graph.id(next)%graph_node_size==graph.id(target))
    {
        if (tarc==targets.size())
        {
            cout<<" TARGET FINALE raggiunto, mi fermo"<<endl;
            stop=true;
            return false;
        }
        else
        {
            int id=targets[tarc];
            tarc++;
            target=graph.nodeFromId(id);
            cout<<time<<": TARGET raggiunto, nuovo target: "<<id<<" "<<(coord_x)[target]<<" "<<(coord_y)[target]<<endl;
        }
    }
    return true;
}