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
						   simulation_time& time
  						)
:targets(tarlist),events(events),
        events_to_index(events_to_index),
        length(graph),coord_x(graph),coord_y(graph),identifier(identifier),communicator(_mutex,&info,_io_service),time(time)
{
    using namespace lemon;
	Graph_creator c(graph,length,coord_x,coord_y);
	graph_node_size=c.createGraph(MAXFLOORS);
	if (!graph_node_size)
		ERR("attenzione, impossibile creare il grafo",0);
    tarc=0;
    int id=targets[tarc];
    tarc++;
    source=graph.nodeFromId(id);
    id=targets[tarc];
    tarc++;
    target=graph.nodeFromId(id);
    routeLock=false;
    next=source;
    xtarget=(coord_x)[next];
    ytarget=(coord_y)[next];
    info[identifier].timestamp=0;
    setTargetStop(false);
	communicator.startReceive();
	speed=5;
	last_time_updated=time;
	stop=false;
	next_target_available=false;
	
}


void agent_router::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
    symbol_table.add_variable("XTARGET",xtarget,false);
    symbol_table.add_variable("YTARGET",ytarget,false);
	symbol_table.add_variable("v_router",speed,false);
}


void agent_router::run_plugin()
{
	if (stop) 
	{
		setTargetStop(true);
		return;
	}
    if (checkIfTargetReached())
    {
        if (setNextTarget())
        {
            next_target_available=true;
        }
        else
        {
			next_target_available=false;
        }
       
		
		communicator.send(); //TODO: invio le informazioni solo quando sono sopra un nodo
    }
    setTargetStop(!next_target_available);
	

}

void agent_router::setTargetStop(bool stop)
{
    events.at(events_to_index.at("STOPPED"))=stop;
    events.at(events_to_index.at("STARTED"))=!stop;
}


bool agent_router::checkIfTargetReached()
{
    return events.at(events_to_index.at("REACHED"));
}


void agent_router::setSource(lemon::SmartDigraphBase::Node s)
{
    source=s;
}

void agent_router::setTarget(lemon::SmartDigraphBase::Node t)
{
    target=t;
}


void agent_router::prepare_info_packet()
{
	using namespace lemon;
	arc_id.clear();
	node_id.clear();
	int j=0;
	for (Path<SmartDigraph>::ArcIt a(p);a!=INVALID;++a)
	{
		j++;
		if (j>2)
			break; //TODO: prenoto solo i prossimi 2 archi del mio percorso
		if (graph.id(graph.source(a))/graph_node_size>MAXFLOORS)
			break;
		arc_id.push_back(graph.id(a));
	}
	j=0;
	for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
	{
		//Il nodo al piano zero (quello iniziale ) e quello al piano finale non vanno inclusi
		if (graph.id(i)/graph_node_size>MAXFLOORS || graph.id(i)<graph_node_size)
			continue;
		j++;
		if (j>2)
			break; //TODO: prenoto solo i prossimi 2 nodi del mio percorso
		node_id.push_back(graph.id(i));
	}
	_mutex.lock();
	graph_informations& tmp = info.at(identifier);
	tmp.id=identifier;
	tmp.isLocked=routeLock;
	tmp.lockedArc=arc_id;
	tmp.lockedNode=node_id;
	tmp.timestamp=last_time_updated;
	_mutex.unlock();
}

bool agent_router::merge_informations_collided(lemon::SmartDigraph::ArcMap<bool>& useArc)
{
	using namespace lemon;
	bool collision=false;
	_mutex.lock();
	for (graph_packet::const_iterator it=info.begin();it!=info.end();it++)
	{
		if (it->first.compare(identifier)==0) continue; //Ignoro me stesso
        bool isLocked = (*it).second.isLocked;
		if (!isLocked) continue;
		int age=round((round(time*1000.0)-round((*it).second.timestamp*1000.0))/1000.0/TIME_SLOT_FOR_3DGRAPH);
		if ((age==0)&&(it->second.id.compare(identifier)>0)) continue; //ignoro le prenotazioni di livello più basso della stessa età
		//TODO: qui sto ignorando gli archi della lista delle prenotazioni che finiscono in un nodo all'ultimo piano
		//cout<<" "<<age<<" ";
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

bool agent_router::findPath()
{
    using namespace lemon;
    if (target==INVALID) return false;
    int real_distance;
	bool reached=dijkstra(graph,length).path(p).dist(real_distance).run(source,target); //senza prenotazioni o con prenotazioni vecchie?
	last_time_updated=round(trunc(time)/TIME_SLOT_FOR_3DGRAPH)*TIME_SLOT_FOR_3DGRAPH; //even if i am late, i need to send the perfect time
	routeLock=true;
	prepare_info_packet();
	communicator.send();
	usleep(1000);
	_io_service.poll();
	_io_service.reset();
	usleep(10000);//TODO: be sure that info are received, but how? _io_service.run?
	while (reached)
	{
		SmartDigraph::ArcMap<bool> useArc(graph,true);
		if (!merge_informations_collided(useArc))
			break;
		cout<<time<<" :collisione trovata,ricalcolo il percorso"<<endl;
		reached= dijkstra(filterArcs(graph,useArc),length).path(p).dist(d).run(source,target);
		if (d>15000)//TODO: questo numero è fisso ma dovrebbe essere una variabile, comunque fa coppia con i 10000 del graph_creator
			reached=false;
		
		if (!reached)
		{
			arc_id.clear();
			node_id.clear();
			for (unsigned int i=0;i<MAXFLOORS-1;i++)
				node_id.push_back(graph.id(source)%graph_node_size+i*graph_node_size);
			
		}
		last_time_updated+=0.0001;
		prepare_info_packet();
		communicator.send();
		usleep(1000);
		_io_service.poll();
		_io_service.reset();
		usleep(10000);
	}
	if (!reached)
		return false;
	simulation_time delta=time-trunc(trunc(time)/TIME_SLOT_FOR_3DGRAPH)*TIME_SLOT_FOR_3DGRAPH; //delta dovrebbe essere tra 0 e 1
	if (delta>TIME_SLOT_FOR_3DGRAPH/3.0)
	{
		cout<<"calcolo velocità. time="<<time<<" delta="<<delta<<endl;
		return false;
		//delta=delta-TIME_SLOT_FOR_3DGRAPH; //Se sono così tanto in ritardo allora salto il turno, se sono in anticipo aspetto
		//TODO: purtroppo per saltare il turno non basta fare così, è un errore. Bisogna stare fermi e aspettare oppure prenotare i nodi ai piani giusti		
	}
	next=graph.target(graph.arcFromId(arc_id[0]));
	xtarget=(coord_x)[next];
	ytarget=(coord_y)[next];
	
	double floor=graph.id(next)/graph_node_size;
	if (floor<0.000001) //floor==0 nei double
		speed=0;
	else
	{
		cout<<"calcolo velocità. time="<<time<<" delta="<<delta<<" piani="<<floor<<" lunghezza="<<length[graph.arcFromId(arc_id[0])];
		speed=1.05*(1.0/floor*(double)length[graph.arcFromId(arc_id[0])])/(-delta+TIME_SLOT_FOR_3DGRAPH*floor);
	}
	std::cout<<"velocità: "<<speed <<" percorso calcolato:";
	for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
		std::cout<<graph.id(i)<<">>";
	std::cout<<" fine"<< std::endl;
	routeLock=true;
	last_time_updated=time;
	return true;
    
}

std::pair< int, int > agent_router::getTargetCoords()
{
    return std::pair<int,int> ((coord_x)[next], (coord_y)[next]);
}

void agent_router::setGraph(lemon::SmartDigraph& g)
{
    lemon::digraphCopy<lemon::SmartDigraph,lemon::SmartDigraph>(g,graph); //graph=g;
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
		//usleep(generatorRandom.integer(0,200)*1000);
        int id=targets[tarc];
        tarc++;
        target=graph.nodeFromId(id);
        cout<<" TARGET raggiunto, nuovo target: "<<id<<" "<<(coord_x)[target]<<" "<<(coord_y)[target]<<endl;
    }
    else
    {
        source=graph.nodeFromId(graph.id(next)%graph_node_size);
// 		cout<<"sleeping "<<identifier.c_str()[identifier.size()-1]<<endl;
// 		usleep(identifier.c_str()[identifier.size()-1]*100000);
    }
    return findPath();
}

ostream& agent_router::toFile(ostream& out) const
{
    using namespace lemon;
    for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
        out<<" "<<(coord_x)[i]<<" "<<(coord_y)[i];
    if (target!=INVALID)
        out<<" "<<(coord_x)[target]<<" "<<(coord_y)[target];
    out<<endl;
    return out;
}

agent_router::~agent_router()
{
}

