#include "identifier_module.h"
#include "../shared/debug_constants.h"
#include <thread>

using namespace std;



identifier_module::identifier_module ( Parsed_World const& W, const std::map<int, double> & sensed_bonus_variables, const std::map<std::string, int> & map_bonus_variables_to_id,
                                       std::shared_ptr<agent_namespace::world_communicator_abstract>& comm, const simulation_time& sensed_time, std::string owner ) :
    parsed_world ( W ),owner ( owner ),agent_world_comm ( comm ),  agent_packet ( last_sensed_agents.bonus_variables,last_sensed_agents.time ),sensed_bonus_variables ( sensed_bonus_variables ),
    map_bonus_variables_to_id ( map_bonus_variables_to_id ),sensed_time ( sensed_time ),real_semaphore1(buffer_lenght),
real_semaphore2(0)
{

    cicli_plugin=0;

    communicator = std::make_shared<agent_to_dummy_communicator>();

    simulate_thread=new std::thread ( &identifier_module::simulate,std::ref ( *this ),std::ref ( sensed_state_agents ), std::ref ( real_semaphore1 ), std::ref ( real_semaphore2 ),
                                      std::ref ( mutex_simulate_variable_access ),std::ref ( parsed_world ),std::ref ( communicator ),std::ref ( old_sensed_agents ),
                                      std::ref ( identifier_matrix ),std::ref ( owner ) );

}





void identifier_module::addReservedVariables ( exprtk::symbol_table< double >& /*arg1*/ )
{

}



void identifier_module::compileExpressions ( exprtk::symbol_table< double >& /*symbol_table*/ )
{

}




// bool identifier_module::agentStatesAreConsistent ( const agent_state& first, const agent_state& second )
// {
//     for ( auto it = first.begin(); it != first.end(); it++ )
//     {
//         if ( abs ( it->second - second.at ( it->first ) ) > tol )
//             return false;
//
//     }
//     return true;
// }

void identifier_module::updateLastSensedAgents()
{
  	for ( auto it = map_bonus_variables_to_id.begin(); it != map_bonus_variables_to_id.end(); ++it )
    {
        last_sensed_agents.bonus_variables[it->first]=sensed_bonus_variables.at ( it->second ) ;
    }

    last_sensed_agents.time = sensed_time;
    last_sensed_agents.state_agents.internal_map = agent_world_comm->get_last_received().state_agents.internal_map;

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

    cicli_plugin++;
    if ( mon_debug_mode == 1 )
    {
        std::cout<<"Run_plugin: ciclo "<<cicli_plugin<<std::endl;
    }


    if ( cicli_plugin < 5 ){
	updateLastSensedAgents();
        return; //parto dopo un po' ma leggo comunque dai sensori
    }
    
    //aggiorno old_state_agents con il pacchetto ricevuto al passo precedente

    
    

    //creo il pacchatto che sara spedito con le informazioni riferite al passo precedente

    if (real_semaphore1.try_wait())
    {
      real_semaphore1.post();
	throw "errore, simula e' troppo lento, ho riempito il buffer";
    }
    
    agent_packet.state_agents.internal_map.clear();
    for ( auto agent= last_sensed_agents.state_agents.internal_map.begin(); agent!=last_sensed_agents.state_agents.internal_map.end(); agent++ )
    {
        agent_packet.state_agents.internal_map[agent->first]=& ( agent->second );
    }

    mutex_simulate_variable_access.lock();
    old_sensed_agents.push_back ( last_sensed_agents );

    //aggiorno il pacchetto con l'ultimo valore ricevuto dai sensori
    updateLastSensedAgents();

    sensed_state_agents.push_back ( last_sensed_agents.state_agents.internal_map );
    communicator->send ( agent_packet );
    mutex_simulate_variable_access.unlock();


real_semaphore2.post();


 mutex_simulate_variable_access.lock();
    if ( old_sensed_agents.size() >100 )
    {
        ERR ( "IdentifierModule::Simulate troppo lenta" , NULL );
        throw ( "IdentifierModule::Simulate troppo lenta" );
    }

mutex_simulate_variable_access.unlock();
  

}


void identifier_module::simulate ( std::list<std::map<std::string,agent_state_packet>> &sensed_agents,named_semaphore& real_semaphore1  , 
				named_semaphore& real_semaphore2,  std::mutex& mutex_simulate_variable_access, 
                                   const Parsed_World & parsed_world, std::shared_ptr<agent_to_dummy_communicator> & communicator,
                                   std::list<world_sim_packet> &old_sensed, std::map <std::string,std::vector< bool >> &identifier_matrix,
                                   std::string& own )

{

    const std::string owner=own;
    std::map<std::string,agent_state_packet> sensed_state_agents;
    world_sim_packet old_sensed_agents;
    std::map<std::string,std::forward_list<std::unique_ptr<dummy_agent>>> sim_agents;
    std::map <std::string,int> index_behaviors;
    std::vector<dynamic*> dynamics;
    agent_state state_reference;
    control_command temp_command;

    monitor_packet packet_viewer;
    packet_viewer.id = owner;


    identifierModule_communicator<monitor_packet>* viewer_comm;
    viewer_comm = new identifierModule_communicator<monitor_packet> ( packet_viewer, owner );


    unsigned i=0;

for ( auto const & behavior: parsed_world.behaviors )
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


    unsigned ncicli=0;
    while ( 1 )
    {
	
	real_semaphore2.wait();
	
	sleep(0.5);
	
	sensed_state_agents.clear();
	mutex_simulate_variable_access.lock();
	for (auto agent:sensed_agents.front() )
	  sensed_state_agents[agent.first]=agent.second;
	
	sensed_agents.pop_front();
        
	old_sensed_agents=old_sensed.front();
	old_sensed.pop_front();
        mutex_simulate_variable_access.unlock();

	
        ncicli++;


        if ( mon_debug_mode == 1 )
        {
            std::cout<<"Simulate:inizio ciclo "<<ncicli<<std::endl;
        }

       
        
        //Elimino gli agenti che non vedo più
    for ( auto & agent_name: sim_agents )
        {
            if ( sensed_state_agents.find ( agent_name.first ) == sensed_state_agents.end() )
            {
                agent_name.second.clear();
            }
        }



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

                    if ( mon_debug_mode == 1 )
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
                        bool state_consistant=true;
                        for ( auto it = new_state.begin(); it != new_state.end(); it++ )
                        {
                            if ( abs ( it->second - sensed_state_agents.at ( agent->first ).state.at ( it->first ) ) > tol )
                            {
                                state_consistant=false;

                            }

                        }
                        if ( state_consistant==true )
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
                    mutex_simulate_variable_access.lock();
                    identifier_matrix.at ( agent->first ).at ( ( *dummy_ref )->behavior_identifier ) = false;
                    mutex_simulate_variable_access.unlock();
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

        if ( !identifier_matrix.empty() && mon_debug_mode == 1 )
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

        //Creo i nuovi dummy per gli agenti appena entrati
    for ( const auto & sensed_agent: sensed_state_agents )
        {
            if ( sim_agents.find ( sensed_agent.first ) == sim_agents.end() && sensed_agent.first != owner )
            {
                //create_agents ( sensed_agent.first, sensed_agent.second.state );
                //il controllo se devo creare o meno l'agente (se e' gia stato escluso per un beaavior non devo crearlo) viene fatto dentro la
                //la funzione create_agents
                assert ( sim_agents[sensed_agent.first].empty() );
                if ( identifier_matrix[sensed_agent.first].empty() )
                {
                    mutex_simulate_variable_access.lock();
                    identifier_matrix[sensed_agent.first].resize ( index_behaviors.size(), true );
                    mutex_simulate_variable_access.unlock();
                }

            for ( auto const & behavior: parsed_world.behaviors )
                {
                    if ( identifier_matrix[sensed_agent.first].at ( index_behaviors.at ( behavior.first ) ) )
                    {
                        dummy_agent* tmp_agent = new dummy_agent ( sensed_agent.first, behavior, index_behaviors.at ( behavior.first ),
                                parsed_world, std::dynamic_pointer_cast<agent_namespace::world_communicator_abstract> ( communicator ),
                                sensed_agent.second.state );

                        sim_agents[sensed_agent.first].push_front ( std::unique_ptr<dummy_agent> ( tmp_agent ) );


                    }

                }

            }
        }

        //

        //SPEDISCO PACCHETTO OUTPUT
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

        mutex_simulate_variable_access.lock();
        communicator->removeFront();
        mutex_simulate_variable_access.unlock();

        if ( mon_debug_mode == 1 )
        {
            std::cout<<"Simulate:fine ciclo "<<ncicli<<std::endl;
        }
	
	real_semaphore1.post();
       
    }

    for ( unsigned int i = 0; i < dynamics.size(); i++ )
        delete dynamics[i];
}

identifier_module::~identifier_module()
{
}

