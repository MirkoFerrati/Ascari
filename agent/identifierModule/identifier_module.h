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
#include <condition_variable>

#define tol 1
#define mon_debug_mode 1
#define update_after 5
#define buffer_lenght 100
#define buffer_refill


class identifier_module: public Plugin_module
{

public:
    void addReservedVariables (exprtk::symbol_table< double >& arg1);
    void compileExpressions (exprtk::symbol_table< double >& arg1);
    void run_plugin();
    void simulate(std::list<std::map<std::string,agent_state_packet>> &sensed_agents, std::mutex& mutex_sem1, std::mutex& mutex_sem2 ,
		  std::mutex& mutex_simulate_variable_access, std::condition_variable &condition_simulate,
		const Parsed_World & parsed_world, std::shared_ptr<agent_to_dummy_communicator> & communicator,std::list<world_sim_packet> &old_sensed_agents, 
		std::map <std::string,std::vector< bool >> &identifier_matrix, std::string& owner);
    void updateLastSensedAgents();
    identifier_module (Parsed_World const& W, const std::map<int,double> & sensed_bonus_variables, const std::map<std::string,int> & map_bonus_variables_to_id,
    std::shared_ptr<agent_namespace::world_communicator_abstract>& comm, const simulation_time & sensed_time,std::string owner);
    ~identifier_module();
    

private:
    //void create_agents(std::string, const agent_state& );
    //bool agentStatesAreConsistent(const agent_state& first, const agent_state& second);
    

    const Parsed_World & parsed_world;
    std::string owner;
    
    
    std::list<world_sim_packet> old_sensed_agents;
    world_sim_packet last_sensed_agents;
    
    
    std::shared_ptr<agent_to_dummy_communicator> communicator;
    std::shared_ptr<agent_namespace::world_communicator_abstract>& agent_world_comm;
    
    
    
    std::map <std::string,std::vector< bool >> identifier_matrix;
    
    
    agent_sim_packet agent_packet;
    
    
    unsigned cicli_plugin;
    
    const std::map<int,double> & sensed_bonus_variables;
    const std::map<std::string,int> & map_bonus_variables_to_id;
    const simulation_time & sensed_time;
   
    
    std::list<std::map<std::string,agent_state_packet>> sensed_state_agents;
    std::thread* simulate_thread;
    std::mutex mutex_sem1;
    std::mutex mutex_sem2;
    std::mutex mutex_simulate_variable_access;
    std::condition_variable condition_simulate;
    
    
    
};




#endif // IDENTIFIER_MODULE_H
