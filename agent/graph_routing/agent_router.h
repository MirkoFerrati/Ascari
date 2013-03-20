#ifndef AGENT_ROUTER_H
#define AGENT_ROUTER_H
#include <lemon/smart_graph.h>
#include <lemon/path.h>
#include <vector>
#include <mutex>


#include <lemon/random.h>
#include "plugin_module.h"
#include <typedefs.h>
#include "udp_graph_communicator.h"
#include <boost/thread.hpp>

#define TIME_SLOT_FOR_3DGRAPH 10.0


enum  class state
{
    MOVING,
    LISTENING,
    HANDSHAKING,
    EMERGENCY,
    STOPPED
};

class agent_router: public Plugin_module
{

public:
    agent_router ( std::vector< int > tarlist, std::map< transition, Events >& events,
                   const std::map<std::string,transition>& events_to_index, std::string identifier,
                   simulation_time& time,std::string graphName );
    void setGraph ( lemon::SmartDigraph& g );
    void run_plugin();
    void addReservedVariables ( exprtk::symbol_table< double >& symbol_table );
    void compileExpressions ( exprtk::symbol_table< double >& symbol_table );
    void setSource ( lemon::SmartDigraph::Node s );
    void setTarget ( lemon::SmartDigraph::Node t );
    std::pair<int,int> getTargetCoords();
    ~agent_router();

private:
    bool findPath ( lemon::DigraphExtender< lemon::SmartDigraphBase >::ArcMap< bool >& useArc );
    void setNextTarget();
    void setSpeed();
    void stopAgent ();
    void startAgent ();
    bool target_reached();
    void prepare_move_packet();
    void prepare_emergency_packet();

    bool detect_collision();
    void filter_graph ( lemon::SmartDigraph::ArcMap< bool >& useArc );
    bool check_for_overtaking( );
    void update_packet();
    bool isTimeToNegotiate();
    void print_path();

private:
    state internal_state;
    bool next_target_reachable;
    int negotiation_steps;
    double xtarget, ytarget,speed;   //usato dal plugin per controllare il robot
    exprtk::expression<double> distance_to_target;  //usato dal plugin per impostare le velocità
    std::map< transition, Events >& events;
    const std::map<std::string,transition>& events_to_index;

    /**Informazioni del grafo*/
    lemon::Path<lemon::SmartDigraph> computed_path;
    lemon::SmartDigraph::ArcMap<int> length;
    lemon::SmartDigraph::NodeMap<int> coord_x, coord_y;
    lemon::SmartDigraph graph; //grafo completo
    int graph_node_size;

    /**Informazioni sullo stato attuale del grafo*/
    graph_informations my_LRP;
    std::vector<int> targets;  //lista dei targets
    unsigned int target_counter; //avanzamento dei target
    std::vector<int> node_id;  //nodi della path
    lemon::SmartDigraph::Node source, target, next; //informazioni locali sui nodi

    /**Informazioni temporali*/
    simulation_time next_time;
    simulation_time &time;
    simulation_time last_time_updated;

    /**Comunicatore*/
    std::mutex _mutex;
    boost::asio::io_service _io_service;
    Udp_graph_communicator communicator;
    std::string priority;
    std::string identifier;
    graph_packet info;

};

#endif // AGENT_ROUTER_H

