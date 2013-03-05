#ifndef IDENTIFIER_MODULE_H
#define IDENTIFIER_MODULE_H
#include <plugin_module.h>
#include <agent.h>
#include <yaml_parser.h>
#include "typedefs.h"
#include <dynamic.h>
#include <forward_list>
#include "agent_to_dummy_communicator.hpp"
#include "identifierModule_communicator.hpp"
#include "dummy_agent.hpp"

#define tol 0.25
#define mon_debug_mode 1
#define update_after 10


class identifier_module: public Plugin_module
{

public:
    void addReservedVariables (exprtk::symbol_table< double >& arg1);
    void compileExpressions (exprtk::symbol_table< double >& arg1);
    void run_plugin();
    identifier_module (Parsed_World const& W, const std::map<int,double> & sensed_bonus_variables, const std::map<std::string,int> & map_bonus_variables_to_id,
    const std::map<std::string,agent_state_packet> &sensed_state_agents, const simulation_time & sensed_time,std::string owner);
    ~identifier_module();
    

private:
    void create_agents(std::string, const agent_state& );
	const Parsed_World & parsed_world;
    
    std::string owner;
    std::shared_ptr<agent_to_dummy_communicator> communicator;
    std::map<std::string,std::forward_list<std::unique_ptr<dummy_agent>>> sim_agents;
    std::map <std::string,int> index_behaviors;
    std::map <std::string,std::vector< bool >> identifier_matrix;
    world_sim_packet old_sensed_agents;
    agent_sim_packet agent_packet;//TODO CONTROLLARE QUI
    const std::map<int,double> & sensed_bonus_variables;
    const std::map<std::string,int> & map_bonus_variables_to_id;
    const std::map<std::string,agent_state_packet> &sensed_state_agents;
    const simulation_time & sensed_time;
    std::vector<dynamic*> dynamics;
    agent_state state_reference;
    control_command temp_command;
    monitor_packet packet_viewer;
    
    //questo per ora non serve
   // monitor_packet buffer_received;
    
    identifierModule_communicator<monitor_packet>* viewer_comm;
    
    std::map<std::string,std::mutex> mutexes;
    bool agentStatesAreConsistent(const agent_state& first, const agent_state& second);
    void printDebug();
    unsigned ncicli;
};




#endif // IDENTIFIER_MODULE_H
