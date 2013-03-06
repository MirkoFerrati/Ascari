#include "identifier_module.h"
#include "../shared/debug_constants.h"

using namespace std;



identifier_module::identifier_module ( Parsed_World const& W, const std::map<int, double> & sensed_bonus_variables, const std::map<std::string, int> & map_bonus_variables_to_id,
                                       const std::map<std::string, agent_state_packet> &sensed_state_agents, const simulation_time& sensed_time, std::string owner ) :
  parsed_world ( W ),owner ( owner ), sensed_bonus_variables ( sensed_bonus_variables ), map_bonus_variables_to_id ( map_bonus_variables_to_id ), sensed_state_agents ( sensed_state_agents ), sensed_time ( sensed_time ), agent_packet(old_sensed_agents.bonus_variables,old_sensed_agents.time)
{
  int i = 0;
  ncicli = 0;
  communicator = std::make_shared<agent_to_dummy_communicator>();
  viewer_comm = new identifierModule_communicator<monitor_packet> ( packet_viewer, owner );
  packet_viewer.id = owner;

for ( auto const & behavior: W.behaviors )
    {
      index_behaviors[behavior.second->name] = i;
      i++;


      for ( unsigned int j = 0; j < behavior.second->state.size(); j++ )
        {
          state_reference.insert ( make_pair ( j, 0 ) );
        }

      //Al simulatore non deve mai arrivare piu' di un controllo per agente, percio' la mappa avra' un solo elemento

      for ( unsigned int j = 0; j < behavior.second->inputs.size(); j++ )
        {
          temp_command.insert ( make_pair ( j, 0 ) );


        }


      dynamic* d = new dynamic ( state_reference, temp_command,
                                 behavior.second->expressions, behavior.second->state, behavior.second->inputs, 10 * T_CAMP );

      dynamics.push_back ( d );
    }





}





void identifier_module::addReservedVariables ( exprtk::symbol_table< double >& /*arg1*/ )
{

}



void identifier_module::compileExpressions ( exprtk::symbol_table< double >& /*symbol_table*/ )
{

}


void identifier_module::create_agents ( std::string agent_name, const agent_state& ref_state )
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

  assert ( sim_agents[agent_name].empty() );
  if ( identifier_matrix[agent_name].empty() )
    {
      identifier_matrix[agent_name].resize ( index_behaviors.size(), true );
    }

for ( auto const & behavior: parsed_world.behaviors )
    {
      if ( identifier_matrix[agent_name].at ( index_behaviors.at ( behavior.first ) ) )
        {
          dummy_agent* tmp_agent = new dummy_agent ( agent_name, behavior, index_behaviors.at ( behavior.first ), parsed_world, std::dynamic_pointer_cast<agent_namespace::world_communicator_abstract> ( communicator ), ref_state );

          sim_agents[agent_name].push_front ( std::unique_ptr<dummy_agent> ( tmp_agent ) );


        }

    }
}

bool identifier_module::agentStatesAreConsistent ( const agent_state& first, const agent_state& second )
{
  for ( auto it = first.begin(); it != first.end(); it++ )
    {
      if ( abs ( it->second - second.at ( it->first ) ) > tol )
        return false;

    }
  return true;
}

void identifier_module::printDebug()
{

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
  assert ( sensed_time - old_sensed_agents.time > 0.001 );
  ncicli++;
  if ( ncicli > 2 )
    {//salto alcuni cicli iniziali. In fondo aggiorno le strutture dati che mi servono
     

  if ( mon_debug_mode == 1 )
    {
      std::cout << "Modulo Identificatore" << ncicli << std::endl;
    }

    
  //Elimino gli agenti che non vedo più
for ( auto & agent_name: sim_agents )
    {
      if ( sensed_state_agents.find ( agent_name.first ) == sensed_state_agents.end() )
        {
          agent_name.second.clear();
        }
    }
    
    communicator->send (agent_packet);

  //Evolvo gli agenti
  for ( auto agent = sim_agents.begin(); agent != sim_agents.end(); ++agent )
    {
      //state_reference = old_sensed_agents.state_agents.internal_map.at (agent->first).state;
      //Evolvo i dummy di ogni agente
      auto old_dummy_ref = agent->second.before_begin(); //uso una forward list, devo tenermi un puntatore di riserva
      for ( auto dummy_ref = agent->second.begin(); dummy_ref != agent->second.end(); ++dummy_ref )
        {
          if ( ( ncicli % update_after ) == 0 )
            {
              for ( unsigned i = 0; i < state_reference.size(); i++ )
                state_reference[i] = old_sensed_agents.state_agents.internal_map.at ( agent->first ).state.at ( i );
              ( *dummy_ref )->getDummyStates().sort();
              ( *dummy_ref )->getDummyStates().unique();
            }
          //Evolvo il singolo dummy
          std::forward_list<dummy_state> tmp_states;
	  
	 
          //communicator->send ( &agent_packet ); lo porto fuori tanto è uguale per tutti
          for ( auto dummystate = ( *dummy_ref )->getDummyStates().begin(); dummystate != ( *dummy_ref )->getDummyStates().end(); ++dummystate )
            {
              if ( ! ( ( ncicli % update_after ) == 0 ) )
                {
                  for ( unsigned i = 0; i < state_reference.size(); i++ )
                    state_reference[i] = ( *dummystate ).state.at ( i );
                }
              {
                std::cout << "Stato Partenza:" << ( *dummystate ).automatonState << std::endl;
                std::cout << "Riferimento : " << state_reference.at ( 0 ) << " " << state_reference.at ( 1 ) << " " << state_reference.at ( 2 ) << endl;
              }
              ( *dummy_ref )->dummy.setDiscreteState ( ( *dummystate ).automatonState );
              ( *dummy_ref )->dummy.main_loop();
              auto control_command_packet = communicator->receive_control_command ( ( *dummy_ref )->identifier );
              for ( auto dstate = ( *dummy_ref )->dummy.getDiscreteStates().begin(); dstate != ( *dummy_ref )->dummy.getDiscreteStates().end(); ++dstate )
                {
                  //preparo il comando relativo allo stato discreto iesimo
                  for ( unsigned i = 0; i < control_command_packet.commands.at ( *dstate ).size(); i++ )
                    {
                      temp_command[i] = control_command_packet.commands.at ( *dstate ).at ( i );
                    }
                  agent_state new_state = dynamics.at ( ( *dummy_ref )->behavior_identifier )->getNextState();
                  //controllo se l'evoluzione e' coerente
                  if ( mon_debug_mode == 1 )
                    {

                      std::cout << "Agent:" << agent->first << " Behavior:" << ( *dummy_ref )->behavior_identifier << std::endl;
                      std::cout << ( *dstate ) << std::endl;
                      std::cout << "Stato Vero:" << sensed_state_agents.at ( agent->first ).state.at ( 0 ) << " " << sensed_state_agents.at ( agent->first ).state.at ( 1 ) << " " << sensed_state_agents.at ( agent->first ).state.at ( 2 ) << endl;
                      std::cout << "Stato Stimato: " << new_state.at ( 0 ) << " " << new_state.at ( 1 ) << " " << new_state.at ( 2 ) << endl;
                    }
                  if ( agentStatesAreConsistent ( new_state, sensed_state_agents.at ( agent->first ).state ) )
                    {
                      dummy_state tmp_state;
                      tmp_state.state = new_state;
                      tmp_state.automatonState = ( *dstate );
                      tmp_states.push_front ( tmp_state );
                    }
                }
            }

          if ( tmp_states.empty() )
            {
              identifier_matrix.at ( agent->first ).at ( ( *dummy_ref )->behavior_identifier ) = false;
              agent->second.erase_after ( old_dummy_ref );
              dummy_ref = old_dummy_ref;
            }
          else
            {
              ( *dummy_ref )->getDummyStates().swap ( tmp_states );
              ++old_dummy_ref;
            }
        }
    }

  if ( mon_debug_mode == 1 )
    {
      std::cout << "Classifier " << std::endl;
      for ( auto agent = identifier_matrix.begin(); agent != identifier_matrix.end(); ++agent )
        {
          std::cout << agent->first << ":";

          for ( unsigned int j = 0; j < agent->second.size(); ++j )
            {
              std::cout << " " << agent->second.at ( j );
            }
          std::cout << std::endl;
        }
    }
  for ( auto agent = identifier_matrix.begin(); agent != identifier_matrix.end(); ++agent )
    {
      unsigned tmp_class = 0;
      std::string tmp_behavior="";
      for ( auto behavior= index_behaviors.begin(); behavior!=index_behaviors.end(); ++behavior )
        {
          if ( agent->second.at ( behavior->second ) ==true )
            {
              tmp_behavior=behavior->first;
              tmp_class++;
            }
        }
      monitor_result tmp_res;
      tmp_res.agent_id=agent->first;
      if ( tmp_class==0 )
        tmp_res.ok=false;
      if ( tmp_class==1 )
        {
          tmp_res.ok=true;
          tmp_res.behavior_name=tmp_behavior;
        }
      packet_viewer.agents.push_front ( tmp_res );
    }
  viewer_comm->send();
  packet_viewer.agents.clear();
  //Creo i nuovi dummy per gli agenti appena entrati
for ( const auto & sensed_agent: sensed_state_agents )
    {
      if ( sim_agents.find ( sensed_agent.first ) == sim_agents.end() && sensed_agent.first != owner )
        {
          create_agents ( sensed_agent.first, sensed_agent.second.state );
          //il controllo se devo creare o meno l'agente (se e' gia stato escluso per un beaavior non devo crearlo) viene fatto dentro la
          //la funzione create_agents
        }
    }
    
    }
    
  //Aggiorno la struttura dati old_sensed_agents
  for ( auto it = map_bonus_variables_to_id.begin(); it != map_bonus_variables_to_id.end(); ++it )
    {
      old_sensed_agents.bonus_variables.insert ( make_pair ( it->first, sensed_bonus_variables.at ( it->second ) ) );
    }

  old_sensed_agents.time = sensed_time;
  old_sensed_agents.state_agents.internal_map = sensed_state_agents;
  
  agent_packet.state_agents.internal_map.clear();
  for (auto agent= old_sensed_agents.state_agents.internal_map.begin();agent!=old_sensed_agents.state_agents.internal_map.end();agent++){
  agent_packet.state_agents.internal_map[agent->first]=&(agent->second);
    
}}

identifier_module::~identifier_module()
{

  for ( unsigned int i = 0; i < dynamics.size(); i++ )
    delete dynamics[i];


}

