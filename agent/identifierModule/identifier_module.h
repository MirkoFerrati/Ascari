#ifndef IDENTIFIER_MODULE_H
#define IDENTIFIER_MODULE_H
#include <plugin_module.h>
#include <agent.h>
#include <yaml_parser.h>
#include "typedefs.h"
#include <dynamic.h>
#include <forward_list>
#include "agent_to_dummy_communicator.hpp"

struct dummy_agent
{
  dummy_agent(std::string agent_name,const std::pair<const std::string, std::unique_ptr<Parsed_Behavior>>& behavior,int behavior_id)
  :dummy(agent_name,true,behavior.second),behavior_identifier(behavior_id),identifier(agent_name)
  {
    char temp[5];
    snprintf(temp,5,"%d",behavior_id);
    identifier.append(temp);
  }
  agent dummy;
  int behavior_identifier;
  std::string identifier;
};


class identifier_module: public Plugin_module
{

public:
    void addReservedVariables (exprtk::symbol_table< double >& arg1);
    void compileExpressions (exprtk::symbol_table< double >& arg1);
    void run_plugin();
    identifier_module (Parsed_World const& W, world_sim_packet const& sensed_agents);
    
    

private:
    void create_agents(std::string agent_name);
	const Parsed_World &parsed_world;
	
    agent_to_dummy_communicator communicator;
    std::map<std::string,std::forward_list<std::unique_ptr<dummy_agent>>> sim_agents;
    std::map <std::string,int> index_behaviors;
    std::map <std::string,std::vector< bool >> identifier_matrix;
    world_sim_packet old_sensed_agents;
    world_sim_packet const & sensed_agents;
    std::vector<dynamic*> dynamics;
    agent_state old_temp_state;
    control_command temp_command;
};




#endif // IDENTIFIER_MODULE_H
