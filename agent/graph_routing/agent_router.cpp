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

lemon::Random agent_router::generatorRandom;

using namespace std;

agent_router::agent_router(std::vector< int > tarlist, std::map< transition, bool >& events, 
						   const std::map<std::string, transition>& events_to_index, string identifier,
						   simulation_time& time
  						)
        :targets(tarlist),events(events),
        events_to_index(events_to_index),
        identifier(identifier),communicator(_mutex,&info,_io_service),length(graph),coord_x(graph),coord_y(graph),time(time)
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
            setTargetStop(false);
        }
        else
        {
            setTargetStop(true);
        }
        _mutex.lock();
		graph_informations& tmp = info.at(identifier);
		tmp.id=identifier;
		tmp.isLocked=routeLock;
		tmp.lockedArc=arc_id;
		tmp.lockedNode=node_id;
		tmp.timestamp=last_time_updated;
		_mutex.unlock();
		
		communicator.send(); //TODO: invio le informazioni solo quando sono sopra un nodo
		cout<<"inviate le informazioni"<<endl;
    }
  

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

bool agent_router::findPath()
{
    using namespace lemon;
    bool reached=false;
    if (target==INVALID) return false;

    SmartDigraph::ArcMap<bool> useArc(graph,true);

    int real_distance;
    dijkstra(graph,length).path(p).dist(real_distance).run(source,target);
    _mutex.lock();
    for (graph_packet::const_iterator it=info.begin();it!=info.end();it++)
    {
		if (it->first.compare(identifier)==0) continue; //Ignoro me stesso
        bool isLocked = (*it).second.isLocked;
        if (!isLocked) continue;
		//TODO: tutto questo codice si può ripensare e ottimizzare pesantemente...un sacco di if inutili ma ora non ho tempo
		if ((time-(*it).second.timestamp>TIME_SLOT_FOR_3DGRAPH)) //Se l'informazione è vecchia scalo tutto
		{
			int age=(time-(*it).second.timestamp)/TIME_SLOT_FOR_3DGRAPH;
			cout<<" "<<age<<" ";
			for (SmartDigraph::ArcIt arc(graph);arc!=INVALID;++arc)
			{
				for (vector<int>::const_iterator itt=(*it).second.lockedNode.begin();itt!=(*it).second.lockedNode.end();itt++)
				{
					if ((*itt)-age*graph_node_size<0) //Se il nodo è finito nel passato lo ignoro
						continue;
					if ((*itt)-age*graph_node_size==graph.id(graph.target(arc)))//TODO: questa riga è talmente fondamentale che ho paura a scriverla
						useArc[arc]=false;
				}
				for (vector<int>::const_iterator itt=(*it).second.lockedArc.begin();itt!=(*it).second.lockedArc.end();itt++)
				{
					if ((*itt)==graph.id(arc))
						;//useArc[arc]=false; //TODO: aiuto, non so cosa scrivere qui
				}
			}
		}
		else //Codice normale
		{
			for (SmartDigraph::ArcIt arc(graph);arc!=INVALID;++arc)
			{
				for (vector<int>::const_iterator itt=(*it).second.lockedNode.begin();itt!=(*it).second.lockedNode.end();itt++)
				{
					if ((*itt)==graph.id(graph.target(arc)))
						useArc[arc]=false;
				}
				for (vector<int>::const_iterator itt=(*it).second.lockedArc.begin();itt!=(*it).second.lockedArc.end();itt++)
				{
					if ((*itt)==graph.id(arc))
						useArc[arc]=false;
				}
			}
		}
    }
    _mutex.unlock();
    reached= dijkstra(filterArcs(graph,useArc),length).path(p).dist(d).run(source,target);
	if (d>15000)//TODO: questo numero è fisso ma dovrebbe essere una variabile, comunque fa coppia con i 10000 del graph_creator
		reached=false;
    if (!reached)
    {
		arc_id.clear();
		node_id.clear();
		for (unsigned int i=0;i<MAXFLOORS-1;i++)
			node_id.push_back(graph.id(source)%graph_node_size+i*graph_node_size);
        return false;
    }
    else
    {
        arc_id.clear();
		node_id.clear();
		int j=0;
        for (Path<SmartDigraph>::ArcIt a(p);a!=INVALID;++a)
		{
			j++;
			arc_id.push_back(graph.id(a));
			if (j>MAXFLOORS)
				break;
		}
		j=0;
        for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
		{
			j++;
			if (j!=1) //Il nodo al piano zero (quello iniziale) non va incluso
			{	
				node_id.push_back(graph.id(i));
			}
			if (j>MAXFLOORS)
				break;
		}
		if (node_id.back()<graph_node_size)//il nodo al piano zero (quello finale) non va incluso
		{
			node_id.pop_back();
		}
		next=graph.target(graph.arcFromId(arc_id[0]));
		xtarget=(coord_x)[next];
		ytarget=(coord_y)[next];
		simulation_time delta=time-trunc(trunc(time)/TIME_SLOT_FOR_3DGRAPH)*TIME_SLOT_FOR_3DGRAPH; //delta dovrebbe essere tra 0 e 1
		if (delta>TIME_SLOT_FOR_3DGRAPH/2.0)
			delta=delta-TIME_SLOT_FOR_3DGRAPH; //Se sono così tanto in ritardo allora salto il turno, se sono in anticipo aspetto
		if ((graph.id(next)/graph_node_size)==0)
			speed=0;
		else
			speed=((double)length[graph.arcFromId(arc_id[0])])/(TIME_SLOT_FOR_3DGRAPH-delta+TIME_SLOT_FOR_3DGRAPH*(double)((graph.id(next)/graph_node_size)-1));
        std::cout<<"velocità: "<<speed <<" percorso calcolato:";
        for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
            std::cout<<graph.id(i)<<">>";
        std::cout<<" fine"<< std::endl;
        routeLock=true;
		last_time_updated=time;
        return true;
    }
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

