#ifndef AGENT_ROUTER_H
#define AGENT_ROUTER_H
#include <lemon/smart_graph.h>
#include <lemon/path.h>
#include <vector>


#include <lemon/random.h>
#include "plugin_module.h"
#include <typedefs.h>

class agent_router: public Plugin_module
{
	
public:
    agent_router(std::vector< int > tarlist, std::map< transition, bool >& events, const index_map& events_to_index, std::string identifier);
    void setGraph(lemon::SmartDigraph& g);
    std::pair<int,int> getTargetCoords();
    void run_plugin();
	bool findPath(const std::vector< graph_informations >& info);
    bool setNextTarget(const std::vector< graph_informations >& info);
    void setSource(lemon::SmartDigraph::Node s);
    void setTarget(lemon::SmartDigraph::Node t);
    void setMapLength(lemon::SmartDigraph::ArcMap<int> m);
    int getLockedArc();
    int getLockedNode();
    ~agent_router();
    /*! 
     * Stampa su out la lista dei target provvisori e infine il target definitivo
     */
    std::ostream& toFile(std::ostream& out) const;
	
	//TODO: we will not expose this informations after communication is enabled
    bool routeLock;
	
private:
	lemon::SmartDigraph graph;
    lemon::SmartDigraph::Node source, target;  
    lemon::SmartDigraph::Node next;
    lemon::SmartDigraph::ArcMap<int> *length;
    lemon::SmartDigraph::NodeMap<int> *coord_x, *coord_y;
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
	const index_map& events_to_index;
    std::map< transition, bool >& events;
    std::string& identifier;

	void setTargetStop(bool stop);
	bool checkIfTargetReached();
	void parseGraph();
};

#endif // AGENT_ROUTER_H
