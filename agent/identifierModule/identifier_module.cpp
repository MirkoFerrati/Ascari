#include "identifier_module.h"
using namespace std;

identifier_module::identifier_module (const Parsed_World& W, const map< string, agent_state_packet >& sensed_agents):
sensed_agents(sensed_agents)
{
     std::map <std::string,int> index_behaviors;
    std::map <std::string,std::vector< bool >> identifier_matrix;

    int i=0;
    for (Parsed_Behavior behavior:W.behaviors)
    {
	index_behaviors[behavior.name]=i++;
    }
    
}





void identifier_module::addReservedVariables (exprtk::symbol_table< double >& arg1)
{

}



void identifier_module::compileExpressions (exprtk::symbol_table< double >& arg1)
{

}


void identifier_module::create_agents(){
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
   * Elimino gli agenti che non vedo più
   * Controllo se ci sono nuovi agenti
   * Creo i dummy per i nuovi agenti
   * Evolvo gli agenti che già vedevo
   * Controllo se l'evoluzione è coerente
   * Elimino i dummy non coerenti
   * Aggiorno la struttura dati old_sensed_agents
   */
  

  old_sensed_agents=sensed_agents;
    
}
