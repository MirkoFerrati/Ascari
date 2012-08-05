#ifndef AGENT_ROUTER_H
#define AGENT_ROUTER_H
#include <lemon/smart_graph.h>
#include <lemon/path.h>
#include <vector>


#include <lemon/random.h>
#include "plugin_module.h"
#include <typedefs.h>
#include "udp_graph_communicator.h"
#include <boost/thread.hpp>

class agent_router: public Plugin_module
{
	
public:
    agent_router(std::vector< int > tarlist, std::map< transition, bool >& events, const std::map<std::string,transition>& events_to_index, std::string identifier);
    void setGraph(lemon::SmartDigraph& g);
    std::pair<int,int> getTargetCoords();
    void run_plugin();
    void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
	
    void setSource(lemon::SmartDigraph::Node s);
    void setTarget(lemon::SmartDigraph::Node t);
//     void setMapLength(lemon::SmartDigraph::ArcMap<int> m);
//     int getLockedArc();
//     int getLockedNode();
    ~agent_router();
    /*! 
     * Stampa su out la lista dei target provvisori e infine il target definitivo
     */
    std::ostream& toFile(std::ostream& out) const;
	
	//TODO: we will not expose this informations after communication is enabled
    bool routeLock;
	
private:
	bool findPath();
    bool setNextTarget();
	int graph_node_size;
	lemon::SmartDigraph graph;
	lemon::SmartDigraph _3Dgraph;
	lemon::SmartDigraph::ArcMap<int> *_3Dlength;
    lemon::SmartDigraph::NodeMap<int> *_3Dcoord_x, *_3Dcoord_y;
    lemon::SmartDigraph::Node source, target;  
    lemon::SmartDigraph::Node next;
    lemon::SmartDigraph::ArcMap<int> *length;
    lemon::SmartDigraph::NodeMap<int> *coord_x, *coord_y;
	double xtarget, ytarget;
    bool pathFound;
    bool graphSet;
    bool targetSet;
    bool sourceSet;
    int d;
    boost::signals2::mutex _mutex;
	boost::asio::io_service _io_service;
    lemon::Path<lemon::SmartDigraph> p;
    static lemon::Random generatorRandom;
    std::vector<int> targets;
    unsigned int tarc;
    std::vector<int> arc_id;
	std::vector<int> node_id;
	const std::map<std::string,transition>& events_to_index;
    std::map< transition, bool >& events;
    std::string& identifier;
	graph_packet info;
	Udp_graph_communicator communicator;
	void setTargetStop(bool stop);
	bool checkIfTargetReached();
	void parseGraph();
	void addFloor(lemon::SmartDigraph::NodeMap< lemon::dim2::Point< int > >& coords,
					lemon::SmartDigraph::NodeMap< int >& ncolors, lemon::SmartDigraph::ArcMap< int >& acolors,
					int startId);
	void finalizeFloor(lemon::SmartDigraph::NodeMap<lemon::dim2::Point<int> >& coords,
	lemon::SmartDigraph::NodeMap<int>& ncolors,
	lemon::SmartDigraph::ArcMap<int>& acolors,int startId);
	void addNodes(lemon::SmartDigraph::NodeMap<lemon::dim2::Point<int> >& coords,
	lemon::SmartDigraph::NodeMap<int>& ncolors,int floorNumber);
	lemon::SmartDigraph::ArcMap<bool> oddArcs,evenArcs;
};

#endif // AGENT_ROUTER_H
