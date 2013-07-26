#include "simulator.h"
#include "../simulator/communication/udp_agent_communicator.h"

#include "logog.hpp"
#include "debug_constants.h"
#include <objects/task_assignment_task.h>
#include <zmq_agent_communicator.h>
#include <zmq_viewer_communicator.hpp>
#include <condition_variable>
#include "collisionchecker.h"
#include "visibility/visibility.h"
#include <map2d.h>
#include "../plugins/agent_router/agent_router_simulator_plugin.h"
#include "../plugins/task_assignment/task_assignment_simulator.h"
#include <time.h>

#include "streams_utils.h"
#include <dynamic_remote_localization.h>
using namespace std;

void simulator::create_communicator ( communicator_types communicator_type,const Parsed_World& world )
{
    if ( communicator_type==communicator_types::SIMULATED_UDP )
    {
        communicator=new udp_agent_communicator ( num_agents );
    }
    else if ( communicator_type==communicator_types::SIMULATED_TCP )
    {
        std::list<std::string> clients;
        for ( auto it=agents_name_to_index.begin(); it!=agents_name_to_index.end(); ++it )
        {
            clients.push_back ( it->first );
            std::cout<<it->first<<std::endl;
        }
        communicator=new zmq_agent_communicator ( num_agents,clients );
    }
    else if (communicator_type == communicator_types::REAL_TCP)
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
}

simulator::simulator() :agent_packet ( sim_packet.bonus_variables,sim_packet.time,sim_packet.object_list )
{
    viewer_communicator=new zmq_viewer_communicator();
    max_loops=0;
    communicator=0;
    num_agents=0;
    world_map=0;
    f_rndom=0;
    cycle_period_millisec=5;
    collisionChecker=0;
    checkCollision=false;


}
void simulator::addPlugin ( abstract_simulator_plugin* plugin )
{
    plugins.push_back ( plugin );
}





void simulator::setPeriod ( unsigned cycle_period_millisec )
{
    this->cycle_period_millisec=cycle_period_millisec;
}

void simulator::setCheckCollision ( bool checkCollision )
{
    this->checkCollision=checkCollision;
    if ( collisionChecker )
        delete collisionChecker;
    if ( checkCollision )
    {
        collisionChecker = new CollisionChecker ( sim_packet.state_agents,agent_states_to_index );
    }
    else
    {
        collisionChecker= new CollisionCheckerAbstract ( sim_packet.state_agents,agent_states_to_index );
    }
}

void simulator::initialize_plugins ( Parsed_World const& wo )
{
for ( auto & plugin:plugins )
        plugin->initialize ( wo );
}


void simulator::initialize ( const Parsed_World& wo )
{

    if ( wo.mapfilename!="UNSET" && wo.mapfilename!="" )
    {
        this->world_map=new map2d ( wo.mapfilename );
    }
    initialize_agents ( wo.agents );
    initialize_plugins ( wo );
    createObjects ( wo );
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
    f_rndom = new rndom<double>();
    bonus_symbol_table.add_function ( f_rndom->name, *f_rndom );
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
    setCheckCollision ( false );
    
    
    localization_receiver.init("simulator");
}


void simulator::createObjects ( const Parsed_World& world )
{
     for ( auto & plugin:plugins )
     {
	    std::list<abstract_object*>* app = plugin->create_objects();
	    
	    if(app!=NULL) sim_packet.object_list.objects[plugin->get_objects_type()] = *app;
	    
	    std::cout<<plugin->get_objects_type()<<" objects : "<<sim_packet.object_list.objects[plugin->get_objects_type()].size()<<std::endl;
	    
	    //for(auto i:sim_packet.object_list.objects[plugin->get_objects_type()])i->print(std::cout);
     }
}


void simulator::initialize_agents ( const list<Parsed_Agent>& agents )
{
    num_agents=agents.size();
    int i=0;
for ( auto ag:agents )
    {

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

        if ( ag.visibility!="" )
        {
            agents_visibility[i]=visibleArea::createVisibilityFromParsedVisibleArea ( ag.visibility,agent_states_to_index );
        }
        i++;
    }

    /*
        for (map<std::string,agent_state_packet>::iterator iter=states_index.internal_map.begin(); iter!=states_index.internal_map.end();iter++)
        {
            cout<<iter->first<<endl;
            for (map<int,double>::iterator iiter=iter->second.state.begin();iiter!=iter->second.state.end();iiter++) {
                //cout<< iiter->first<<"->"<<iiter->second<<endl;
            }
            for (map<string,int>::iterator iiter=agent_states_to_index.at(agent_name_to_index.at(iter->first)).begin();iiter!=agent_states_to_index.at(agent_name_to_index.at(iter->first)).end();iiter++) {
                cout<< iiter->first<<"->"<<iter->second.state.at(iiter->second)<<endl;
            }
        }*/
}

void simulator::input_loop ( mutex& input_mutex,condition_variable& input_cond,volatile bool& paused,volatile bool& exit,volatile int& cycle_period_millisec )
{
    char c;
    string temp;
    bool isReading;
    while ( !exit )
    {
        std::cin.read ( &c,1 );
        if ( isReading )
        {
            temp.push_back ( c );
        }
        if ( c=='p' )
        {
            std::lock_guard<std::mutex> lock ( input_mutex );
//             cout<<"letto un carattere:"<<c<<paused<<endl;

            paused=!paused;
            c='0';
            input_cond.notify_one();
        }
        if ( c=='s' )
        {
            isReading=true;
            temp.clear();
            c='0';
        }
        if ( c=='!' )
        {
            isReading=false;
            std::lock_guard<std::mutex> lock ( input_mutex );
            cout<<"letto un carattere:"<<c<<temp<<atoi ( temp.c_str() ) <<endl;
            cycle_period_millisec=atoi ( temp.c_str() );
            c='0';
            input_cond.notify_one();
        }
    }
}


void simulator::main_loop()
{
    try
    {
        termios before, after;
        tcgetattr ( STDIN_FILENO, &before ); // fill 'before' with current termios values
        after = before;                     // make a copy to be modified
        after.c_lflag &= ( ~ICANON );       // Disable canonical mode, including line buffering
        after.c_lflag &= ( ~ECHO );         // Don't echo characters on the screen (optional)
        tcsetattr ( STDIN_FILENO, TCSANOW, &after ); // Set the modified flags

        std::mutex input_mutex;
        std::condition_variable input_cond;
        volatile bool input_exit=false;
        volatile bool paused=false;
        std::thread input_thread ( &simulator::input_loop,this,std::ref ( input_mutex ),std::ref ( input_cond ),std::ref ( paused ),std::ref ( input_exit ),std::ref ( cycle_period_millisec ) );

        sim_packet.time=0;
        int clock=0;
	struct timespec requestStart, requestEnd;
	double accum;
        while ( !s_interrupted )
        {
	    clock_gettime ( CLOCK_REALTIME, &requestStart );

            {
                sleep ( 0 );
                std::unique_lock<std::mutex> lock ( input_mutex );
                while ( paused )
                {
                    //cout<<"sono dentro il while della condition_variable"<<endl;
                    input_cond.wait ( lock );
                    // cout<<paused<<endl;
                }
            }
            clock++;
            if ( ( clock%10 ) !=0 )
            {
                cout<<".";
                flush ( cout );
            }
            else
                cout<<endl<<sim_packet.time;
            sim_packet.time= ( ( simulation_time ) clock ) /10.0;
//             communicator->send_broadcast(time++);
            update_bonus_variables();

            for ( auto agent=sim_packet.state_agents.internal_map.begin(); agent!=sim_packet.state_agents.internal_map.end(); agent++ )
            {
                agent_packet.state_agents.internal_map.clear();
                for ( auto other=sim_packet.state_agents.internal_map.begin(); other!=sim_packet.state_agents.internal_map.end(); other++ )
                {
                    if ( agent->first==other->first ||
                            !agents_visibility.count ( agents_name_to_index.at ( agent->first ) ) ||
                            !agents_visibility.count ( agents_name_to_index.at ( other->first ) ) ||
                            (
                                agents_visibility.at ( agents_name_to_index.at ( agent->first ) )->isVisible ( agent->second.state,other->second.state )
                                && ( !world_map || world_map->isVisible ( agent->second.state,other->second.state ) )
                            )
                       )
                    {
                        agent_packet.state_agents.internal_map[other->first]=&other->second;
                    }
                    else
                    {
                        continue;
                    }
                }
                communicator->send_target ( agent_packet,agent->first );
            }
            
            viewer_communicator->send_target ( sim_packet,"viewer" );
// 	    cout<<"inviato pacchetto con gli stati"<<endl;


        for ( auto & plugin:plugins )
                plugin->run_plugin();

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

            for ( auto object_list=sim_packet.object_list.objects.begin(); object_list!=sim_packet.object_list.objects.end(); ++object_list )
            {
		for ( auto object:object_list->second )
		{
		    object->updateState ( sim_packet.time,sim_packet.state_agents,agent_states_to_index );
		}
            }

            collisionChecker->checkCollisions();
            vector<control_command_packet> temp=communicator->receive_control_commands();


//             cout<<"ricevuto pacchetto con i controlli"<<endl;
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
	    if ((cycle_period_millisec-accum)*1000-10 >0)
	      usleep ( (cycle_period_millisec-accum)*1000-10 );
            //cout<<endl<< "tempo necessario:"<<endl;
            //printf( "%f\n", accum );
        }

    for ( auto & plugin:plugins )
            plugin->stop();
        input_exit=true;
        tcsetattr ( STDIN_FILENO, TCSANOW, &before );

    }
    catch ( const char* e )
    {
        ERR ( "%s",e );
    }
}



simulator::~simulator()
{
    sim_packet.time=-10;
    if ( communicator )
        communicator->send_broadcast ( agent_packet );


    delete communicator;

//     for (unsigned int i=0; i< map_bonus_variables.size();i++)
//         bonus_expressions.at(i).~expression();

    for ( unsigned int i=0; i<dynamic_modules.size(); i++ )
        delete dynamic_modules[i];


    delete f_rndom;


    delete world_map;
    delete collisionChecker;
}

void simulator::start_sim ( int max_loops )
{
    this->max_loops=max_loops;
    time=0;
    main_loop();
}


void simulator::update_bonus_variables()
{
    for ( map<std::string,double>::iterator it=sim_packet.bonus_variables.begin(); it!=sim_packet.bonus_variables.end(); ++it )
    {
        it->second=bonus_expressions.at ( map_bonus_variables.at ( it->first ) ).value();
    }
//Copio i risultati alla fine per avere coerenza all'interno dello stesso clock
    for ( map<string,double>::iterator it=sim_packet.bonus_variables.begin(); it!=sim_packet.bonus_variables.end(); ++it )
    {
        bonusVariables.at ( map_bonus_variables_to_id.at ( it->first ) ) =it->second;
    }

}

