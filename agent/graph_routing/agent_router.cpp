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
#include <lemon/color.cc>
#include "debug_constants.h"


lemon::Random agent_router::generatorRandom;

using namespace std;

agent_router::agent_router(vector<int> tarlist,map<transition,bool>& events, index_map const& events_to_index,string identifier)
        :targets(tarlist),events(events),events_to_index(events_to_index),identifier(identifier)
{
    using namespace lemon;
    tarc=0;
    int id=targets[tarc];
    tarc++;
    target=graph.nodeFromId(id);
    routeLock=false;
    next=source;
    arc_id=-1;
}

void agent_router::parseGraph()
{
    coord_x= new lemon::SmartDigraph::NodeMap<int>(graph);
    coord_y= new lemon::SmartDigraph::NodeMap<int>(graph);
    length= new lemon::SmartDigraph::ArcMap<int>(graph);

    try {
        lemon::digraphReader(graph, GRAPHNAME). // read the directed graph into g
        nodeMap("coordinates_x", *coord_x).	//read the coordinates of nodes
        nodeMap("coordinates_y", *coord_y).	//read the coordinates of nodes
        arcMap("length", *length).       // read the 'capacity' arc map into cap
        run();
    } catch (lemon::Exception& er) { // check if there was any error
       ERR("parsing exception %s",er.what());
    }

    std::cout << "A digraph is read from "<<GRAPHNAME << std::endl;
    std::cout << "Number of nodes: " << lemon::countNodes(graph) << std::endl;
    std::cout << "Number of arcs: " << lemon::countArcs(graph) << std::endl;
}



void agent_router::run_plugin()
{
        graph_informations tmp;
        tmp.id=identifier;
        tmp.isLocked=routeLock;
        tmp.lockedArc=getLockedArc();
        tmp.lockedNode=getLockedNode();


        //TODO questa istruzione sincronizza tutti gli agenti allo stesso istante ogni ciclo di clock
//         communicator->send_and_receive(tmp);


//         router_output<<fixed<<setprecision(1)<<setw(2)<<time<<" ";
//         router->toFile(router_output);
        //se ho raggiunto il target intermedio attuale

        if (checkIfTargetReached())
        {//se riesco a raggiungere il prossimo target intermedio
//             if (setNextTarget(communicator->getInfo()))//TODO: settare i valori di xtarget e ytarget, e new_target a true
            {
                setTargetStop(false);
            }
//             else
            {
                setTargetStop(true);
            }
        }

}

void agent_router::setTargetStop(bool stop)
{
// 	events.at(events_to_index.at("stopped"))=stop;
// 	events.at(events_to_index.at("started"))=stop;
}


bool agent_router::checkIfTargetReached()
{
// 	return events.at(events_to_index.at("reached"));
}


void agent_router::setSource(lemon::SmartDigraphBase::Node s)
{
    source=s;
}

void agent_router::setTarget(lemon::SmartDigraphBase::Node t)
{
    target=t;
}

bool agent_router::findPath(const std::vector< graph_informations >& info)
{
    using namespace lemon;
    bool reached=false;
    if (target==INVALID) return false;
    
    SmartDigraph::ArcMap<bool> useArc(graph,true);
    
    int real_distance;
    dijkstra(graph,*length).path(p).dist(real_distance).run(source,target);
    
	for (vector<graph_informations>::const_iterator it=info.begin();it!=info.end();it++)
    {
		bool isLocked = (*it).isLocked;
		int lockedNode =(*it).lockedNode;
		int lockedArc = (*it).lockedArc;
		//int lockedNode = (*it)->getLockedNode();
		//int lockedArc = (*it)->getLockedArc();
		//bool isLocked = (*it)->routeLock();
      if (!isLocked) continue;
        for(SmartDigraph::OutArcIt arc1(graph, source);arc1!=INVALID;++arc1)
	{
	    if (lockedNode==graph.id(graph.target(arc1)))
	      useArc[arc1]=false;
	    if (lockedArc==graph.id(arc1))
	      useArc[arc1]=false;
        }
        
    }

    reached= dijkstra(filterArcs(graph,useArc),*length).path(p).dist(d).run(source,target);

    if (d>real_distance+2) return false;
    
    if (!reached)
    {
        return false;
    }
    else
    {
        //std::cout<<"prossimo nodo:"<<graph->id(next);
	Path<SmartDigraph>::ArcIt a(p);
	arc_id=graph.id(a);
	next=graph.target(a);
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
    return std::pair<int,int> ((*coord_x)[next], (*coord_y)[next]); 
}

//TODO: we will not expose this informations after communication is enabled
int agent_router::getLockedNode()
{
    return graph.id(next);
}
//TODO: we will not expose this informations after communication is enabled
int agent_router::getLockedArc()
{
	return arc_id;
}
void agent_router::setGraph(lemon::SmartDigraph& g)
{
lemon::digraphCopy<lemon::SmartDigraph,lemon::SmartDigraph>(g,graph); //graph=g;
}



bool agent_router::setNextTarget(const std::vector< graph_informations >& info)
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
        cout<<" TARGET raggiunto, nuovo target: "<<id<<" "<<(*coord_x)[target]<<" "<<(*coord_y)[target]<<endl;
    }
    else
    {
      source=next;
    }
  return findPath(info);
}

void agent_router::setMapLength(lemon::DigraphExtender< lemon::SmartDigraphBase >::ArcMap< int > m)
{
    lemon::mapCopy<lemon::SmartDigraph>(graph,m,*length);
}

ostream& agent_router::toFile(ostream& out) const
{
  using namespace lemon;
  for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
            out<<" "<<(*coord_x)[i]<<" "<<(*coord_y)[i];
  if (target!=INVALID)
    out<<" "<<(*coord_x)[target]<<" "<<(*coord_y)[target];
  out<<endl;
  return out;
}

agent_router::~agent_router()
{
}

