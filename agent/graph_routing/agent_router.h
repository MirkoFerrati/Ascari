#ifndef AGENT_ROUTER_H
#define AGENT_ROUTER_H
#include <lemon/smart_graph.h>
#include <lemon/path.h>
#include <vector>


#include <lemon/random.h>
#include "tcp_router.h"

class Agent;


class agent_router
{
    lemon::SmartDigraph *graph;
    lemon::SmartDigraph::Node source, target;  

    lemon::SmartDigraph::Node next;

    lemon::SmartDigraph::ArcMap<int> length;
    lemon::SmartDigraph::NodeMap<int> coord_x, coord_y;
    bool pathFound;
    bool graphSet;
    bool targetSet;
    bool sourceSet;
    int d;
    lemon::Path<lemon::SmartDigraph> p;
    static lemon::Random generatorRandom;
    std::vector<int> targets;
    unsigned int tarc;
    int arc_id;

public:
    agent_router(lemon::SmartDigraph *g, lemon::SmartDigraph::ArcMap<int> *m,
		 lemon::SmartDigraph::Node s, 
		 lemon::SmartDigraph::NodeMap<int> *x,lemon::SmartDigraph::NodeMap<int> *y,
		 std::vector<int> tarlist
		);
    agent_router(const agent_router& other);
    virtual ~agent_router();
    void setGraph(lemon::SmartDigraph *g);
    std::pair<int,int> getTargetCoords();
    bool findPath(const std::vector< graph_informations >& info);
    bool setNextTarget(const std::vector< graph_informations >& info);
    void setSource(lemon::SmartDigraph::Node s);
    void setTarget(lemon::SmartDigraph::Node t);
    void setMapLength(lemon::SmartDigraph::ArcMap<int> m);
    int getLockedArc();
    int getLockedNode();
    
    /*! 
     * Stampa su out la lista dei target provvisori e infine il target definitivo
     */
    std::ostream& toFile(std::ostream& out) const;
	
	//TODO: we will not expose this informations after communication is enabled
    bool routeLock;
    
    
    
};

#endif // AGENT_ROUTER_H
