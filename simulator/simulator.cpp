#include "simulator.h"
#include "../simulator/communication/udp_agent_communicator.h"

#include "logog.hpp"
#include "debug_constants.h"
// #include <objects/task_assignment_task.h>
#include <zmq_agent_communicator.h>
#include <zmq_viewer_communicator.hpp>
#include <condition_variable>
#include "collisionchecker.h"
#include "visibility/visibility.h"
#include <map2d.h>
// #include "../plugins/agent_router/agent_router_simulator_plugin.h"
// #include "../plugins/task_assignment/task_assignment_simulator.h"
#include <time.h>
#include <utility>
#include "streams_utils.h"

#include <dynamic_remote_localization.h>
using namespace std;

void simulator::create_communicator ( communicator_types communicator_type,const Parsed_World& world )
{
    if ( communicator_type==communicator_types::SIMULATED_UDP )
    {
        communicator=std::make_shared<udp_agent_communicator> ( num_agents );
    }
    else if ( communicator_type==communicator_types::SIMULATED_TCP )
    {
        std::list<std::string> clients;
        for ( auto it=agents_name_to_index.begin(); it!=agents_name_to_index.end(); ++it )
        {
            clients.push_back ( it->first );
            std::cout<<it->first<<std::endl;
        }
        communicator=std::make_shared<zmq_agent_communicator> ( num_agents,clients );
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
        communicator=std::make_shared<zmq_agent_communicator> ( clients.size(),clients );
        //communicator=std::make_shared<simulator_namespace::agent_communicator_abstract>(new zmq_agent_communicator ( clients.size(),clients )); //GCC BUG TRIGGER
    }
}

void simulator::set_communicator(shared_ptr< simulator_namespace::agent_communicator_abstract > communicator)
{
    this->communicator=communicator;
}


simulator::simulator() :agent_packet ( sim_packet.bonus_variables,sim_packet.time,sim_packet.object_list )
{
    viewer_communicator=new zmq_viewer_communicator();
    max_loops=0;
//     communicator=0;
    num_agents=0;
    world_map=0;
    f_rndom=0;
    cycle_period_millisec=100;
    collisionChecker=0;
    checkCollision=false;
    timeSimulated=true;
    T_integration=0.01;
    if (CONFIG.exists("T_INTEGRATION"))
        T_integration=atof(CONFIG.getValue("T_INTEGRATION").c_str());

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

    simulated_sensors=CONFIG.exists("SIMULATED_SENSORS") && atoi(CONFIG.getValue("SIMULATED_SENSORS").c_str());
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

        this->agent_packet.state_agents.internal_map.insert(std::make_pair(ag.name,std::ref(agent_packet)));//Inizialize with all the agent states, we may need this later
        
        
        //We will not use a simulated time if there are any real agents in the environment
        if (!ag.simulated)
        {
            timeSimulated=false;
        }

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
            command_packet.command.insert ( make_pair ( j,0 ) );
            commands_to_index_tmp.insert ( make_pair ( ag.behavior->inputs.at ( j ),j ) );
            bonus_symbol_table.add_variable ( ag.behavior->inputs.at ( j ) +ag.name,command_packet.command.at ( j ) );
        }
        agent_commands_to_index.push_back ( commands_to_index_tmp );

        dynamic_module_abstract *d;

        if (ag.simulated)
        {
            d= new dynamic ( sim_packet.state_agents.internal_map.at ( ag.name ).state, commands.at ( ag.name ).command,
                             ag.behavior->expressions, ag.behavior->state,ag.behavior->inputs,T_integration );
        }
        else
        {
             d= new dynamic_remote_localization(&localization_receiver,ag.name,sim_packet.state_agents.internal_map.at ( ag.name ).state);
        }
        dynamic_modules.push_back ( d );

        if ( ag.visibility!="" )
        {
            agents_visibility[i]=visibleArea::createVisibilityFromParsedVisibleArea ( ag.visibility,agent_states_to_index );
        }
        i++;
    }
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


termios simulator::initializeKeyboardInput()
{
    termios before, after;
    tcgetattr ( STDIN_FILENO, &before ); // fill 'before' with current termios values
    after = before;                     // make a copy to be modified
    after.c_lflag &= ( ~ICANON );       // Disable canonical mode, including line buffering
    after.c_lflag &= ( ~ECHO );         // Don't echo characters on the screen (optional)
    tcsetattr ( STDIN_FILENO, TCSANOW, &after ); // Set the modified flags
    return before;
}

void simulator::printTime(int clock)
{

    if ( ( clock%10 ) !=0 )
    {
        cout<<".";
        flush ( cout );
    }
    else
        cout<<endl<<sim_packet.time;
}


bool simulator::isVisible(map< string, agent_state_packet >::iterator agent, map< string, agent_state_packet >::iterator other)
{
    return (agent->first==other->first ||
            !agents_visibility.count ( agents_name_to_index.at ( agent->first ) ) ||
            !agents_visibility.count ( agents_name_to_index.at ( other->first ) ) ||
            (
                agents_visibility.at ( agents_name_to_index.at ( agent->first ) )->isVisible ( agent->second.state,other->second.state )
                && ( !world_map || world_map->isVisible ( agent->second.state,other->second.state ) )
            ));
}

void simulator::main_loop()
{
    try
    {
       // termios before=initializeKeyboardInput();

        // Start input thread
        std::mutex input_mutex;
        std::condition_variable input_cond;
        volatile bool input_exit=false;
        volatile bool paused=false;
//         std::thread input_thread ( &simulator::input_loop,this,std::ref ( input_mutex ),std::ref ( input_cond ),std::ref ( paused ),std::ref ( input_exit ),std::ref ( cycle_period_millisec ) );

        //Setup timing variables
        simulation_time old_time=0;
        sim_packet.time=0;
        int clock=0;
        struct timespec requestStart, requestEnd, simulationStart;
        clock_gettime ( CLOCK_REALTIME, &simulationStart );
        double total_accum, step_accum;

        //Main infinite Loop
        while ( !s_interrupted )
        {
            clock_gettime ( CLOCK_REALTIME, &requestStart );

            //Check for input loop
            {
//                 sleep ( 0 );
//                 std::unique_lock<std::mutex> lock ( input_mutex );
//                 while ( paused )
//                 {
//                     input_cond.wait ( lock );
//                 }
            }
            clock++;
            //printTime(clock);

            //Update simulation time inside sim_packet
            if (timeSimulated)
            {
                sim_packet.time= ( ( simulation_time ) clock ) /10.0;
            }
            else
            {
                total_accum= ( requestStart.tv_sec - simulationStart.tv_sec )*1000
                             + ( requestStart.tv_nsec - simulationStart.tv_nsec )/ 1E6;
                sim_packet.time=total_accum/1000.0;
            }

            update_bonus_variables();

            if (simulated_sensors)
            {
                //Compute new agent simulated sensors result
                for ( auto agent=sim_packet.state_agents.internal_map.begin(); agent!=sim_packet.state_agents.internal_map.end(); agent++ )
                {
                    agent_packet.state_agents.internal_map.clear();
                    for ( auto other=sim_packet.state_agents.internal_map.begin(); other!=sim_packet.state_agents.internal_map.end(); other++ )
                    {
                        if (isVisible(agent,other))
                        {
                            agent_packet.state_agents.internal_map.insert(std::make_pair(other->first,std::ref(other->second)));
                        }
                    }
                    communicator->send_target ( agent_packet,agent->first );
                }
            } 
            else
            {
                communicator->send_broadcast(agent_packet);
            }
            viewer_communicator->send_target ( sim_packet,"viewer" );

            for ( auto & plugin:plugins )
                plugin->run_plugin();

            //Compute new agents continuous state
            agent_state state_tmp;
            //TODO(Mirko): attenzione, si perdono passi di integrazione per colpa del round, uno dovrebbe salvarsi il resto del round e usarlo al passo successivo
            //unsigned int integration_times=round(max(step_accum/1000.0,cycle_period_millisec/1000.0)/T_integration);
            unsigned int integration_times=round((sim_packet.time-old_time)/T_integration);
            old_time=sim_packet.time;
//             cout<<"integration_times:"<<integration_times<<endl;
            for ( unsigned int i=0; i<integration_times; i++ )
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

            //Update objects state
            for ( auto object_list=sim_packet.object_list.objects.begin(); object_list!=sim_packet.object_list.objects.end(); ++object_list )
            {
                for ( auto object:object_list->second )
                {
                    object->updateState ( sim_packet.time,sim_packet.state_agents,agent_states_to_index );
                }
            }

            collisionChecker->checkCollisions();

            //Receive and save control commands
            vector<control_command_packet>& temp=communicator->receive_control_commands();//WARN TODO CHECK THIS REFERENCE FOR MUTEX!!!!
            for ( unsigned i=0; i< temp.size(); i++ )
            {
                for ( map<int,double>::iterator it=commands.at ( temp.at ( i ).identifier ).command.begin(); it!=commands.at ( temp.at ( i ).identifier ).command.end(); ++it )
                {
                    it->second=temp.at(i).command.at(it->first);
                }
            }

            if ( sim_packet.time>max_loops )
                break;
            // Calculate time taken by a request
            clock_gettime ( CLOCK_REALTIME, &requestEnd );

//             Calculate time it took
            step_accum = ( requestEnd.tv_sec - requestStart.tv_sec )*1000
                         + ( requestEnd.tv_nsec - requestStart.tv_nsec )/ 1E6;
            if ((cycle_period_millisec-step_accum)*1000-10 >0)
            {
                usleep ( (cycle_period_millisec-step_accum)*1000-10 );
            }
            else
            {
//                 WARN("this timestep %f took too much time: %f",sim_packet.time,step_accum);
            }
        }

        for ( auto & plugin:plugins )
            plugin->stop();
        //sim_packet.time=-10;
        //communicator->send_broadcast(agent_packet);

        input_exit=true;
        //tcsetattr ( STDIN_FILENO, TCSANOW, &before );
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

    if (viewer_communicator)
        delete viewer_communicator;
    
//     delete communicator;

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

