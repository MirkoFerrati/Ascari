#include "identifier_module.h"
using namespace std;

identifier_module::identifier_module (const Parsed_World& W, const  world_sim_packet & sensed_agents):
sensed_agents(sensed_agents)
{
     std::map <std::string,int> index_behaviors;
    std::map <std::string,std::vector< bool >> identifier_matrix;

    int i=0;
    for (auto behavior:W.behaviors)
    {
	index_behaviors[behavior.second.name]=i;
	i++;
    }
    
    
}





void identifier_module::addReservedVariables (exprtk::symbol_table< double >& arg1)
{

}



void identifier_module::compileExpressions (exprtk::symbol_table< double >& symbol_table)
{
  
}


void identifier_module::create_agents(std::string agent_name){
 /**
  * Piu' o meno cosi':
  * Parsed_World temp_world
  * for (behavior: W.behaviors)
  * {
  * temp_world.behaviors.push_back(behavior);
  * temp_world.agents.push_back(agent_name);
  * dummy=new agent(dummy_name,true,temp_world);
  * sim_agents[agent_name].insert(dummy);
  * }
  *
  */ 
  
}

void identifier_module::run_plugin()
{

  /**in linea generica:
   * Controllo agenti nel vicinato
   * Elimino gli agenti che non vedo più
   * Controllo se ci sono nuovi agenti
   * Creo i dummy per i nuovi agenti
   * Evolvo gli agenti che già vedevo
   * Controllo se l'evoluzione è coerente
   * Elimino i dummy non coerenti
   * Aggiorno la struttura dati old_sensed_agents
   */
  
  //controllo il tempo
  assert(sensed_agents.time-old_sensed_agents.time>0.001);
  
  
  for (auto sensed_agent:sensed_agents.state_agents.internal_map){
     if (sim_agents.find(sensed_agent.first)==sim_agents.end())
     {
       create_agents(sensed_agent.first);
     }
   
    
  }
  
  for (auto agent_name:sim_agents){
     
    if (sensed_agents.state_agents.internal_map.find(agent_name.first)==sensed_agents.state_agents.internal_map.end())
     {
       //remove_dummies(sensed_agent.first);
     }
  }
    
  

  old_sensed_agents=sensed_agents;
    
}
