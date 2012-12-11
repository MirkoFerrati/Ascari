#include "identifier_module.h"
using namespace std;

identifier_module::identifier_module (const Parsed_World& W, const map< string, agent_state_packet >& sensed_agents):
sensed_agents(sensed_agents)
{
    

    
    
}





void identifier_module::addReservedVariables (exprtk::symbol_table< double >& arg1)
{

}



void identifier_module::compileExpressions (exprtk::symbol_table< double >& arg1)
{

}




void identifier_module::run_plugin()
{

  old_sensed_agents=sensed_agents;
}
