#include "agent_router.h"
#include <lemon/dijkstra.h>
#include <lemon/path.h>
#include <lemon/random.h>
#include <time.h>
#include <agent.h>
#include <lemon/adaptors.h>
#include <cassert>

lemon::Random agent_router::generatorRandom;

using namespace std;

agent_router::agent_router(lemon::SmartDigraph* g, lemon::SmartDigraph::ArcMap< int > *m,
                           lemon::SmartDigraph::Node s,
                           lemon::SmartDigraph::NodeMap<int> *x,lemon::SmartDigraph::NodeMap<int> *y,
			   vector<int> tarlist
                          )
        :graph(g),source(s),next(s),length(*g),coord_x(*g),coord_y(*g),targets(tarlist)
{
    using namespace lemon;
    tarc=0;
    int id=targets[tarc];
    tarc++;
    target=g->nodeFromId(id);
    mapCopy<SmartDigraph>(*g,*m,length);
    mapCopy<SmartDigraph>(*g,*x,coord_x);
    mapCopy<SmartDigraph>(*g,*y,coord_y);
    routeLock=false;
    next=source;
    arc_id=-1;
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
    
    SmartDigraph::ArcMap<bool> useArc(*graph,true);
    
    int real_distance;
    dijkstra(*graph,length).path(p).dist(real_distance).run(source,target);
    
	for (vector<graph_informations>::const_iterator it=info.begin();it!=info.end();it++)
    {
		bool isLocked = (*it).isLocked;
		int lockedNode =(*it).lockedNode;
		int lockedArc = (*it).lockedArc;
		//int lockedNode = (*it)->getLockedNode();
		//int lockedArc = (*it)->getLockedArc();
		//bool isLocked = (*it)->routeLock();
      if (!isLocked) continue;
        for(SmartDigraph::OutArcIt arc1(*graph, source);arc1!=INVALID;++arc1)
	{
	    if (lockedNode==graph->id(graph->target(arc1)))
	      useArc[arc1]=false;
	    if (lockedArc==graph->id(arc1))
	      useArc[arc1]=false;
        }
        
    }

    reached= dijkstra(filterArcs(*graph,useArc),length).path(p).dist(d).run(source,target);

    if (d>real_distance+2) return false;
    
    if (!reached)
    {
        return false;
    }
    else
    {
        //std::cout<<"prossimo nodo:"<<graph->id(next);
	Path<SmartDigraph>::ArcIt a(p);
	arc_id=graph->id(a);
	next=graph->target(a);
        std::cout<<"percorso calcolato:";
        for (PathNodeIt<Path<SmartDigraph> > i(*graph,p); i != INVALID; ++i)
            std::cout<<graph->id(i)<<">>";
        std::cout<<" fine"<< std::endl;
	routeLock=true;
        return true;
    }
}

std::pair< int, int > agent_router::getTargetCoords()
{
    return std::pair<int,int> (coord_x[next], coord_y[next]); 
}

//TODO: we will not expose this informations after communication is enabled
int agent_router::getLockedNode()
{
    return graph->id(next);
}
//TODO: we will not expose this informations after communication is enabled
int agent_router::getLockedArc()
{
	return arc_id;
}
void agent_router::setGraph(lemon::SmartDigraph* g)
{
    graph=g;
}



bool agent_router::setNextTarget(const std::vector< graph_informations >& info)
{

    if (graph->id(next)==graph->id(target))
    {
        source=target;
	if (tarc==targets.size())
	{
	  return false;
	}
	int id=targets[tarc];
	tarc++;
        target=graph->nodeFromId(id);
        
        cout<<" TARGET raggiunto, nuovo target: "<<id<<" "<<coord_x[target]<<" "<<coord_y[target]<<endl;
    }
    else
    {
      source=next;
    }
  return findPath(info);
}

void agent_router::setMapLength(lemon::DigraphExtender< lemon::SmartDigraphBase >::ArcMap< int > m)
{
    lemon::mapCopy<lemon::SmartDigraph>(*graph,m,length);
}

ostream& agent_router::toFile(ostream& out) const
{
  using namespace lemon;
  for (PathNodeIt<Path<SmartDigraph> > i(*graph,p); i != INVALID; ++i)
            out<<" "<<coord_x[i]<<" "<<coord_y[i];
  if (target!=INVALID)
    out<<" "<<coord_x[target]<<" "<<coord_y[target];
  out<<endl;
  return out;
}

agent_router::~agent_router()
{
}

