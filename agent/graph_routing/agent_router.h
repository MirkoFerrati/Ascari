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

#define MAXFLOORS 4
#define TIME_SLOT_FOR_3DGRAPH 10.0


class agent_router: public Plugin_module
{
	
public:
    agent_router(std::vector< int > tarlist, std::map< transition, bool >& events, const std::map<std::string,transition>& events_to_index, std::string identifier,simulation_time& time);
    void setGraph(lemon::SmartDigraph& g);
    std::pair<int,int> getTargetCoords();
    void run_plugin();
    void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
	void compileExpressions(exprtk::symbol_table< double >& symbol_table);
    void setSource(lemon::SmartDigraph::Node s);
    void setTarget(lemon::SmartDigraph::Node t);

    ~agent_router();
    /*! 
     * Stampa su out la lista dei target provvisori e infine il target definitivo
     */
    std::ostream& toFile(std::ostream& out) const;
	
private:
	std::vector<int> targets;
    lemon::SmartDigraph graph;
    bool findPath(lemon::DigraphExtender< lemon::SmartDigraphBase >::ArcMap< bool >& useArc);
    bool setNextTarget();
	double xtarget, ytarget;
    bool pathFound;
    bool graphSet;
    bool targetSet;
    bool sourceSet;
	bool isNegotiating;
    int d;
	double speed;
	exprtk::expression<double> distance_to_target;
	simulation_time next_time;
    boost::signals2::mutex _mutex;
	boost::asio::io_service _io_service;
    lemon::Path<lemon::SmartDigraph> p;
    static lemon::Random generatorRandom;
    unsigned int tarc;
    std::vector<int> arc_id;
	std::vector<int> node_id;
	std::map< transition, bool >& events;
	
	const std::map<std::string,transition>& events_to_index;
	lemon::SmartDigraph::ArcMap<int> length;
	lemon::SmartDigraph::NodeMap<int> coord_x, coord_y;
	std::string& identifier;
	graph_packet info;
	Udp_graph_communicator communicator;
	void setTargetStop(bool stop);
	bool checkIfTargetReached();
    void prepare_info_packet();
    bool merge_informations_collided(lemon::SmartDigraph::ArcMap< bool >& useArc);
	lemon::SmartDigraph::Node source, target;
    lemon::SmartDigraph::Node next;
    int graph_node_size;
	simulation_time &time;
	simulation_time last_time_updated;
    bool stop;
    bool next_target_reachable;
	int isWaiting;
};

#endif // AGENT_ROUTER_H

