#ifndef IDENTIFIER_MODULE_H
#define IDENTIFIER_MODULE_H
#include <plugin_module.h>
#include <agent.h>
#include <yaml_parser.h>
#include "typedefs.h"
#include <dynamic.h>
#include <forward_list>

class identifier_module: public Plugin_module
{

public:
    void addReservedVariables (exprtk::symbol_table< double >& arg1);
    void compileExpressions (exprtk::symbol_table< double >& arg1);
    void run_plugin();
    identifier_module (Parsed_World const& W, world_sim_packet const& sensed_agents);
    

private:
    void create_agents(string agent_name);
    std::map<std::string,std::forward_list< agent* >> sim_agents;
    std::map <std::string,int> index_behaviors;
    std::map <std::string,std::vector< bool >> identifier_matrix;
    world_sim_packet old_sensed_agents;
    world_sim_packet const & sensed_agents;
    std::vector<dynamic*> dynamics;
};




#endif // IDENTIFIER_MODULE_H
