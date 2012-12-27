#include "identifier_module.h"
using namespace std;

identifier_module::identifier_module (const Parsed_World& W, const  world_sim_packet& sensed_agents) :
     parsed_world (W), sensed_agents (sensed_agents), communicator()
{
    std::map <std::string, int> index_behaviors;
    std::map <std::string, std::vector< bool >> identifier_matrix;
    int i = 0;
for (auto const & behavior: W.behaviors) {
        index_behaviors[behavior.second->name] = i;
        i++;
    }


}





void identifier_module::addReservedVariables (exprtk::symbol_table< double >& /*arg1*/)
{

}



void identifier_module::compileExpressions (exprtk::symbol_table< double >& /*symbol_table*/)
{

}


void identifier_module::create_agents (std::string agent_name)
{
    /**
     * Piu' o meno cosi':
     * Creo la colonna della matrice relativa all'agente appena creato, se non esisteva gia'
     * Se esisteva, vuol dire che ho gia' fatto un po' di identificazione, quindi ho gia' escluso qualche behavior
     * for (behavior: World.behaviors)
     * {
     *  Se il behavior per l'agente appena creato non era gia' stato escluso, crea un nuovo dummy con quel behavior
     *  Altrimenti vado al prossimo behavior
     * }
     *
     */
    assert(sim_agents[agent_name].empty());
    if (identifier_matrix[agent_name].empty())
	{
	  identifier_matrix[agent_name].resize(index_behaviors.size(),true);
	}
    
for (auto const& behavior: parsed_world.behaviors) {
    if (identifier_matrix[agent_name].at(index_behaviors.at(behavior.first)))
    {
	dummy_agent* tmp_agent=new dummy_agent(agent_name,behavior,index_behaviors.at(behavior.first));
	
	sim_agents[agent_name].push_front(std::unique_ptr<dummy_agent>(tmp_agent));
	
    }
    }

}

void identifier_module::run_plugin()
{

    /**in linea generica:
     * Controllo agenti nel vicinato
     * Elimino gli agenti che non vedo più
     * Evolvo gli agenti che già vedevo
     * Controllo se l'evoluzione e' coerente
     * Elimino i dummy non coerenti
     * Controllo se ci sono nuovi agenti
     * Creo i dummy per i nuovi agenti
     * Aggiorno la struttura dati old_sensed_agents
     */

    //controllo il tempo
    assert (sensed_agents.time - old_sensed_agents.time > 0.001);

    //Elimino gli agenti che non vedo più
for (auto & agent_name: sim_agents) {

        if (sensed_agents.state_agents.internal_map.find (agent_name.first) == sensed_agents.state_agents.internal_map.end()) {
            agent_name.second.clear();
        }
    }


    //Evolvo gli agenti
    for (auto agent = sim_agents.begin(); agent != sim_agents.end(); ++agent) {
        old_temp_state = old_sensed_agents.state_agents.internal_map.at (agent->first).state;
        //Evolvo i dummy di ogni agente
        auto old_dummy_ref = agent->second.before_begin(); //uso una forward list, devo tenermi un puntatore di riserva
        for (auto dummy_ref = agent->second.begin(); dummy_ref != agent->second.end(); ++dummy_ref) {
            ++old_dummy_ref;
            //Evolvo il singolo dummy
            communicator.send ( (**dummy_ref), old_sensed_agents.state_agents);
            (*dummy_ref)->dummy.main_loop();
            auto control_command_packet = communicator.receive ( (**dummy_ref));
            for (auto command = control_command_packet.commands.begin(); command != control_command_packet.commands.end(); ++command) {
                //preparo il comando relativo allo stato discreto iesimo
		temp_command = command->second;
                auto new_state = dynamics.at ( (*dummy_ref)->behavior_identifier)->getNextState();
                //controllo se l'evoluzione e' coerente
                if (new_state != sensed_agents.state_agents.internal_map.at (agent->first).state) {
                    //Elimino i dummy non coerenti
                    (*dummy_ref)->dummy.getDiscreteStates().remove(command->first);
		    if((*dummy_ref)->dummy.getDiscreteStates().empty()){ 
                    identifier_matrix.at (agent->first).at ( (*dummy_ref)->behavior_identifier) = false;
                    agent->second.erase_after (old_dummy_ref);
		    dummy_ref=old_dummy_ref;
		    }
                }
            }
        }
    }


    //Creo i nuovi dummy per gli agenti appena entrati
for (const auto & sensed_agent: sensed_agents.state_agents.internal_map) {
        if (sim_agents.find (sensed_agent.first) == sim_agents.end()) {
            create_agents (sensed_agent.first);
        }
    }



    //Aggiorno la struttura dati old_sensed_agents
    old_sensed_agents = sensed_agents;

}
