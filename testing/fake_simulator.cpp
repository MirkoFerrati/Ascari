#include "fake_simulator.h"
#include "zmq_agent_communicator.h"
#include "logog.hpp"
#include <time.h>

#include "streams_utils.h"
#include <dynamic_remote_localization.h>
#include <types/control_command_packet.h>
#include <dynamic.h>

using namespace std;

void fake_simulator::create_communicator(int type, const Parsed_World& world)
{
        std::list<std::string> clients;
        for ( auto ag:world.agents )
        {
          if (ag.simulated)
          {
            clients.push_back ( ag.name );
            std::cout<<ag.name<<std::endl;
          }
        }
        communicator=new zmq_agent_communicator ( clients.size(),clients );      
}




fake_simulator::fake_simulator() :agent_packet ( sim_packet.bonus_variables,sim_packet.time,sim_packet.object_list )
{
    max_loops=0;
    cycle_period_millisec=50;
}


void fake_simulator::setPeriod ( unsigned cycle_period_millisec )
{
    this->cycle_period_millisec =cycle_period_millisec;
}


void fake_simulator::initialize ( const Parsed_World& wo,std::string agent_name )
{
  this->agent_name=agent_name;
    initialize_agents( wo.agents,agent_name );
    bonus_symbol_table.add_constants();
    int i=0;
    for ( map<bonusVariable,bonus_expression>::const_iterator it=wo.bonus_expressions.begin(); it!=wo.bonus_expressions.end(); ++it )
    {
        bonusVariables[i]=0;
        map_bonus_variables_to_id.insert ( make_pair ( it->first,i ) );
        i++;
    }
    for ( index_map::const_iterator it=map_bonus_variables_to_id.begin(); it!=map_bonus_variables_to_id.end(); ++it )
    {
        bonus_symbol_table.add_variable ( it->first,bonusVariables[it->second] );
    }
    bonus_symbol_table.add_constant ( "PI_GRECO",exprtk::details::numeric::constant::pi );
    exprtk::parser<double> parser;
    for ( unsigned int i=0; i<wo.bonus_variables.size(); i++ )
    {
        sim_packet.bonus_variables.insert ( make_pair ( wo.bonus_variables.at ( i ),0 ) );
        exprtk::expression<double> expression_tmp;
        expression_tmp.register_symbol_table ( bonus_symbol_table );
        string string_tmp_expression=wo.bonus_expressions.at ( wo.bonus_variables.at ( i ) );
        if ( parser.compile ( string_tmp_expression,expression_tmp ) )
        {
            bonus_expressions.push_back ( expression_tmp );
        }
        else
        {
            ERR ( "impossibile creare l'espressione: %s %s",string_tmp_expression.c_str(),parser.error().c_str() );
//             throw "impossibile creare l'espressione";
        }

        map_bonus_variables.insert ( make_pair ( wo.bonus_variables.at ( i ),i ) );
    }
    localization_receiver.init("simulator");
}


void fake_simulator::initialize_agents ( const list<Parsed_Agent>& agents,std::string agent_name )
{
    int i=0;
for ( auto ag:agents )
    {
	if (ag.name!=agent_name)
	  continue;
	agents_name_to_index.insert ( make_pair ( ag.name,i ) );
        agent_state_packet agent_packet_tmp;
        control_command_packet command_packet_tmp;
        sim_packet.state_agents.internal_map.insert ( make_pair ( ag.name,agent_packet_tmp ) );
        commands.insert ( make_pair ( ag.name,command_packet_tmp ) );
        agent_state_packet& agent_packet=sim_packet.state_agents.internal_map.at ( ag.name );
        control_command_packet& command_packet=commands.at ( ag.name );
        agent_packet.identifier=ag.name;
        command_packet.identifier=ag.name;
        index_map commands_to_index_tmp;

        for ( unsigned int j=0; j<ag.behavior->state.size(); j++ )
        {
            agent_packet.state.insert ( make_pair ( j,ag.initial_states.at ( ag.behavior->state.at ( j ) ) ) );
            agent_states_to_index.insert ( make_pair ( ag.behavior->state.at ( j ),j ) );
            bonus_symbol_table.add_variable ( ag.behavior->state.at ( j ) +ag.name,agent_packet.state.at ( j ) );
        }

        //Al simulatore non deve mai arrivare piu' di un controllo per agente, percio' la mappa avra' un solo elemento

        for ( unsigned int j=0; j<ag.behavior->inputs.size(); j++ )
        {
            command_packet.default_command.insert ( make_pair ( j,0 ) );
            commands_to_index_tmp.insert ( make_pair ( ag.behavior->inputs.at ( j ),j ) );
            bonus_symbol_table.add_variable ( ag.behavior->inputs.at ( j ) +ag.name,command_packet.default_command.at ( j ) );
        }
        agent_commands_to_index.push_back ( commands_to_index_tmp );

	dynamic_module_abstract *d;
	if (ag.simulated)
	  d= new dynamic ( sim_packet.state_agents.internal_map.at ( ag.name ).state, commands.at ( ag.name ).default_command,
                                  ag.behavior->expressions, ag.behavior->state,ag.behavior->inputs );
	else
	  d= new dynamic_remote_localization(&localization_receiver,ag.name,sim_packet.state_agents.internal_map.at ( ag.name ).state);
	
        dynamic_modules.push_back ( d );
        i++;
    }

}


void fake_simulator::main_loop()
{
    try
    {
        sim_packet.time=0;
        int clock=0;
	struct timespec requestStart, requestEnd;
	double accum;
        while ( !s_interrupted )
        {
	    clock_gettime ( CLOCK_REALTIME, &requestStart );
                sleep ( 0 );
            clock++;
            if ( ( clock%10 ) !=0 )
            {
                cout<<".";
                flush ( cout );
            }
            else
                cout<<endl<<sim_packet.time;
            sim_packet.time= ( ( simulation_time ) clock ) /10.0;
//            update_bonus_variables();
	    agent_packet.state_agents.internal_map[agent_name]=&(sim_packet.state_agents.internal_map[agent_name]);
            communicator->send_target ( agent_packet,agent_name );
 
            agent_state state_tmp;
            for ( int i=0; i<10; i++ )
            {
                for ( index_map::const_iterator iter=agents_name_to_index.begin(); iter!=agents_name_to_index.end(); ++iter )
                {
                    state_tmp=dynamic_modules.at ( iter->second )->getNextState();
                    for ( agent_state::const_iterator iiter=state_tmp.begin(); iiter!=state_tmp.end(); ++iiter )
                    {
                        sim_packet.state_agents.internal_map.at ( iter->first ).state.at ( iiter->first ) =iiter->second; //metto il nuovo stato nel posto giusto senza copiare i vettori
                    }
                }
            }
            
            vector<control_command_packet> temp=communicator->receive_control_commands();

	    for ( unsigned i=0; i< temp.size(); i++ )
            {

                for ( map<int,double>::iterator it=commands.at ( temp.at ( i ).identifier ).default_command.begin(); it!=commands.at ( temp.at ( i ).identifier ).default_command.end(); ++it )
                {
                    it->second= ( *temp.at ( i ).commands.begin() ).second.at ( it->first );
                }

            }

            if ( sim_packet.time>max_loops )
                break;
            // Calculate time taken by a request
            clock_gettime ( CLOCK_REALTIME, &requestEnd );

//             Calculate time it took
             accum = ( requestEnd.tv_sec - requestStart.tv_sec )*1000
                            + ( requestEnd.tv_nsec - requestStart.tv_nsec )
                            / 1E6;
	    if (cycle_period_millisec-accum >0)
	      usleep ( (cycle_period_millisec-accum)*1000-10 );

            //cout<<endl<< "tempo necessario:"<<endl;
            //printf( "%f\n", accum );
        }

    }
    catch ( const char* e )
    {
        ERR ( "%s",e );
    }
}



fake_simulator::~fake_simulator()
{
    sim_packet.time=-10;    
    delete communicator;
    for ( unsigned int i=0; i<dynamic_modules.size(); i++ )
        delete dynamic_modules[i];
}

void fake_simulator::start_sim ( int max_loops )
{
    this->max_loops=max_loops;
    time=0;
    main_loop();
}

