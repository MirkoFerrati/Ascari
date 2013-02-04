#ifndef IDENTIFIER_MODULE_H
#define IDENTIFIER_MODULE_H
#include <plugin_module.h>
#include <agent.h>
#include <yaml_parser.h>
#include "typedefs.h"
#include <dynamic.h>
#include <forward_list>
#include "agent_to_dummy_communicator.hpp"

#include "dummy_agent.hpp"

class identifier_module: public Plugin_module
{

public:
    void addReservedVariables (exprtk::symbol_table< double >& arg1);
    void compileExpressions (exprtk::symbol_table< double >& arg1);
    void run_plugin();
    identifier_module (Parsed_World const& W, const std::map<int,double> & sensed_bonus_variables, const std::map<std::string,int> & map_bonus_variables_to_id,
    const std::map<std::string,agent_state_packet> &sensed_state_agents, const simulation_time & sensed_time);
    
    

private:
    void create_agents(std::string agent_name);
	const Parsed_World &parsed_world;
	
    agent_to_dummy_communicator communicator;
    std::map<std::string,std::forward_list<std::unique_ptr<dummy_agent>>> sim_agents;
    std::map <std::string,int> index_behaviors;
    std::map <std::string,std::vector< bool >> identifier_matrix;
    world_sim_packet old_sensed_agents;
    const std::map<int,double> & sensed_bonus_variables;
    const std::map<std::string,int> & map_bonus_variables_to_id;
    const std::map<std::string,agent_state_packet> &sensed_state_agents;
    const simulation_time & sensed_time;
    std::vector<dynamic*> dynamics;
    agent_state old_temp_state;
    control_command temp_command;
    std::map<std::string,std::mutex> mutexes;
};




#endif // IDENTIFIER_MODULE_H
