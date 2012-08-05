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



lemon::Random agent_router::generatorRandom;

using namespace std;

agent_router::agent_router(std::vector< int > tarlist, std::map< transition, bool >& events, 
						   const std::map<std::string, transition>& events_to_index, string identifier)
        :targets(tarlist),events(events),
        events_to_index(events_to_index),
        identifier(identifier),communicator(_mutex,&info,_io_service),length(graph),coord_x(graph),coord_y(graph)
{
    using namespace lemon;
	Graph_creator c(graph,length,coord_x,coord_y);
    c.createGraph(MAXFLOORS);
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
}


void agent_router::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
    symbol_table.add_variable("XTARGET",xtarget,false);
    symbol_table.add_variable("YTARGET",ytarget,false);
}


void agent_router::run_plugin()
{
	_mutex.lock();
    graph_informations& tmp = info.at(identifier);
    tmp.id=identifier;
    tmp.isLocked=routeLock;
    tmp.lockedArc=arc_id;
    tmp.lockedNode=node_id;
    tmp.timestamp++;
    _mutex.unlock();

//         router_output<<fixed<<setprecision(1)<<setw(2)<<time<<" ";
//         router->toFile(router_output);
    //se ho raggiunto il target intermedio attuale

    if (checkIfTargetReached())
    {//se riesco a raggiungere il prossimo target intermedio
        if (setNextTarget())
        {
            setTargetStop(false);
        }
        else
        {
            setTargetStop(true);
        }
    }
        communicator.send();


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
// 		int lockedNode =(*it).second.lockedNode;
//         int lockedArc = (*it).second.lockedArc;
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
    _mutex.unlock();
    reached= dijkstra(filterArcs(graph,useArc),length).path(p).dist(d).run(source,target);

//     if (d>real_distance+2) return false;

    if (!reached)
    {
        return false;
    }
    else
    {
        //std::cout<<"prossimo nodo:"<<graph->id(next);
        arc_id.clear();
		int j=0;
        for (Path<SmartDigraph>::ArcIt a(p);a!=INVALID;++a)
		{
			j++;
			arc_id.push_back(graph.id(a));
			if (j>MAXFLOORS-2)
				break;
		}
		j=0;
        for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
		{
			j++;
			node_id.push_back(graph.id(i));
			if (j>MAXFLOORS-2)
				break;
		}
		next=graph.target(graph.arcFromId(arc_id[0]));
		xtarget=(coord_x)[next];
		ytarget=(coord_y)[next];

        std::cout<<"percorso calcolato:";
        for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
            std::cout<<graph.id(i)<<">>";
        std::cout<<" fine"<< std::endl;
        routeLock=true;
        return true;
    }
}

std::pair< int, int > agent_router::getTargetCoords()
{
    return std::pair<int,int> ((coord_x)[next], (coord_y)[next]);
}

// //TODO: we will not expose this informations after communication is enabled
// int agent_router::getLockedNode()
// {
//     return graph.id(next);
// }
// //TODO: we will not expose this informations after communication is enabled
// int agent_router::getLockedArc()
// {
//     return arc_id;
// }
void agent_router::setGraph(lemon::SmartDigraph& g)
{
    lemon::digraphCopy<lemon::SmartDigraph,lemon::SmartDigraph>(g,graph); //graph=g;
}



bool agent_router::setNextTarget()
{

    if (graph.id(next)==graph.id(target))
    {
        source=target;
        if (tarc==targets.size())
        {
            return false;
        }
        int id=targets[tarc];
        tarc++;
        target=graph.nodeFromId(id);
        cout<<" TARGET raggiunto, nuovo target: "<<id<<" "<<(coord_x)[target]<<" "<<(coord_y)[target]<<endl;
    }
    else
    {
        source=next;
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

