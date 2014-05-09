#ifndef AGENT_ROUTER_H
#define AGENT_ROUTER_H

#include "abstract_agent_plugin.h"
#include <boost/circular_buffer.hpp>

#include "HACK_KDEVELOP.h"
// #ifdef ISAGENT

#include <lemon/smart_graph.h>
#include <lemon/path.h>
#include <vector>
#include <mutex>


#include <lemon/random.h>
#include "../../shared/types/graph_informations.h"
#include "udp_graph_communicator.h"
#include "router_memory_communicator.hpp"
#include "agent_router_logging.h"
#include "../agent/agent.h"
#include <thread>

#define TIME_SLOT_FOR_3DGRAPH 10.0


enum class state
{
    MOVING,
    LISTENING,
    NODE_HANDSHAKING,
    ARC_HANDSHAKING,
    EMERGENCY,
    STOPPING,
    STOPPED,
    STARTING,
	LOADING,
};


class agent_router: public abstract_agent_plugin
{

public:
    agent_router ( std::vector< int > tarlist, std::string identifier,
                   simulation_time& time,std::string graphName );
    agent_router( agent* a, Parsed_World* parse );
    void setGraph ( lemon::SmartDigraph& g );
    void run_plugin();
    virtual bool initialize();
    void setSource ( lemon::SmartDigraph::Node s );
    void setTarget ( lemon::SmartDigraph::Node t );
    ~agent_router();

    inline void setTargets(std::vector<int> targets){this->targets=targets;};
    
private:
    bool findPath ( lemon::DigraphExtender< lemon::SmartDigraphBase >::ArcMap< bool >& useArc );
	bool isEmergency(const std::vector<int>& nodes);
    bool setNextTarget();
    void setSpeed();
    void stopAgent ();
    void startAgent ();
	bool isOnTarget();
        
    /*!
     * @return distance_to_control_target()<control_node_radius
     */    
    bool target_reached();
    void prepare_move_packet();
    void prepare_emergency_packet();
    bool detect_collision();
    void filter_graph ( lemon::SmartDigraph::ArcMap< bool >& useArc );
    bool check_for_overtaking( );
    void update_packet();
    bool isTimeToNegotiate(simulation_time time);
    void print_path(std::ostringstream& out);
    int findAge(simulation_time present_time, simulation_time old_time);
    void prepare_stopped_packet();
    void prepare_loading_packet();
    void print_state( state s, std::ostream& out );
    std::string print_state( state s );
        
    simulation_time getNextTime();
    double distance_to_target();
    /*!
     * @return distance_to_target()<node_radius
     */    
    bool isNearNode();
    double distance_to_control_target();
private:
    state internal_state;
    bool next_target_reachable;
    int negotiation_steps;
    double xtarget, ytarget;
    double control_xtarget,control_ytarget;
    double *speed, *omega;   //usato dal plugin per controllare il robot
    double *x,*y,*theta;
    /**Informazioni del grafo*/
        lemon::SmartDigraph graph; //grafo completo

    lemon::Path<lemon::SmartDigraph> computed_path;
    lemon::SmartDigraph::ArcMap<int> length;
    lemon::SmartDigraph::NodeMap<int> coord_x, coord_y;
    int graph_node_size;

    /**Informazioni sullo stato attuale del grafo*/
    graph_informations my_LRP;
    std::vector<int> targets;  //lista dei targets
    unsigned int target_counter; //avanzamento dei target
    std::vector<int> node_id;  //nodi della path
    lemon::SmartDigraph::Node source, target,control_target;//informazioni locali sui nodi
   
    /**Informazioni temporali*/
    //simulation_time next_time;
    simulation_time &time;
    simulation_time last_time_updated;
    simulation_time last_time_left_a_node;
    state old_state;


    //std::vector<std::string> log_buffer;
    boost::circular_buffer<std::string> log_buffer;
    /**Comunicatore*/
//     std::mutex _mutex;  
    std::string identifier;
    graph_packet info;
//     boost::asio::io_service _io_service;
    //Udp_graph_communicator communicator;
    router_memory_manager communicator;
    std::string priority;
    bool started;
    bool initialized;
    agent* a;
    std::string graphName;
    double node_radius;
    double control_node_radius;
    simulation_time last_time_negotiated;
    bool already_received;
    int stopping;
    bool there_was_a_collision;
    
    agent_router_logging logger;
    
};

// #endif
#endif // AGENT_ROUTER_H

