#include "agent.h"
#include "../plugins/agent_router/agent_router.h"

//written by Alessandro Settimi
#ifdef GLPK_FOUND
#include "task_assignment/task_assignment.h"
#endif
//written by Alessandro Settimi

#include "../plugins/monitor/identifier_module.h"
#include <string>
#include <utility>
#include <map>
#include "communication/udp_world_communicator.h"
#include "communication/zmq_world_communicator.h"


#include "logog.hpp"
#include "automaton/automatonFSM.h"
#include "automaton/automatonEFSM.h"
#include "encoder/encoderDet.h"
#include "geometry.hpp"


using namespace std;

agent::agent ( std::string name,const std::unique_ptr<Parsed_Behavior>& behavior, const Parsed_World & world )
    :identifier ( name ),behavior(behavior),world(world),isDummy(true),noStart(false)
{
    encoder=0;
    initialized=false;
}

agent::agent ( const Parsed_World& world, bool noStart ) :world(world),
    identifier ( world.agents.front().name ),noStart(noStart),behavior(world.agents.front().behavior),isDummy(false)
{
    encoder=0;
}

void agent::initialize()
{
      createBonusVariablesFromWorld ( world.bonus_expressions );
    for ( unsigned int i=0; i<plugins.size(); i++ )
    {
        plugins[i]->addReservedVariables ( symbol_table );
        plugins[i]->addReservedVariables ( encoder_symbol_table );
    }
    
    init(behavior,noStart);
    if (isDummy)
      for ( auto const & disc_state : map_discreteStateName_to_id )
        discreteState.push_front ( disc_state.second );
      else
	discreteState.push_front(map_discreteStateName_to_id.at(world.agents.front().state_start));
}

void agent::setCommunicator ( std::shared_ptr<agent_namespace::world_communicator_abstract>& communicator )
{
    world_comm=communicator;
}


void agent::setControlCommandIdentifier ( string new_identifier )
{
    inputs.identifier=new_identifier;
}


void agent::init ( const std::unique_ptr<Parsed_Behavior> & behavior, bool noStart )
{
    time=0;
    symbol_table.add_constants();
    pi=exprtk::details::numeric::constant::pi;
    symbol_table.add_variable ( "PI_GRECO",pi,true );
    f_rndom = new rndom<double>();
    symbol_table.add_function ( f_rndom->name, *f_rndom );

    int i=0;
    for ( map<controller_name,controller_MapRules>::const_iterator it =behavior->controllers.begin(); it !=behavior->controllers.end(); ++it )
    {
        map_controllername_to_id.insert ( make_pair ( it->first,i++ ) );
    }
    event_decoder.setEventsAndSubEvents ( &sub_events,&events );


    createDiscreteStateFromParsedAgent ( behavior );
    createStateFromParsedAgent ( behavior );
    createSubEventsFromParsedAgent ( behavior );
    createEventsFromParsedAgent ( behavior );
    createControllersFromParsedAgent ( behavior );

    if ( !isDummy )
    {
        automaton=new automatonFSM ( createAutomatonTableFromParsedAgent ( behavior ) );
        encoder=new encoderDet ( sub_events, identifier,state,map_statename_to_id,bonusVariables,
                                 map_bonus_variables_to_id, behavior->topology_expressions,
                                 sub_events_to_index,behavior->lambda_expressions,encoder_symbol_table );
        if ( !noStart ) world_comm=std::make_shared<zmq_world_communicator> ( identifier );
    }
    else
    {
        automaton=new automatonEFSM ( createAutomatonTableFromParsedAgent ( behavior ) );
        encoder=new encoderDet ( sub_events, identifier,state,map_statename_to_id,bonusVariables,
                                 map_bonus_variables_to_id, behavior->topology_expressions,
                                 sub_events_to_index,behavior->lambda_expressions,encoder_symbol_table );
    }
    for ( unsigned int i=0; i<plugins.size(); i++ )
    {
        plugins[i]->compileExpressions ( symbol_table );
    }
    event_decoder.create ( behavior->events_expressions,sub_events_to_index,events_to_index );
    initialized=true;
}

void agent::addPlugin ( abstract_agent_plugin* plugin)
{
        plugins.push_back ( plugin );
}

void agent::createBonusVariablesFromWorld ( map< bonusVariable, bonus_expression > bonus )
{
    int i=0;
    for ( map<bonusVariable,bonus_expression>::const_iterator it=bonus.begin(); it!=bonus.end(); ++it )
    {
        bonusVariables[i]=0;
        map_bonus_variables_to_id.insert ( make_pair ( it->first,i ) );
        i++;
    }
    for ( index_map::const_iterator it=map_bonus_variables_to_id.begin(); it!=map_bonus_variables_to_id.end(); ++it )
    {
        symbol_table.add_variable ( it->first,bonusVariables[it->second] );
    }

}

void agent::start()
{
    if ( !initialized )
        throw "please call init function before start";
    try
    {
        inputs.identifier = identifier;
        int cicli = 0;
        std::cout<<"inizio ciclo infinito di agent s_interrupted="<<s_interrupted<<std::endl;
        while ( !s_interrupted )
        {
            main_loop();
            cicli++;
            if ( time < -1 )
                break;
        }

    }
    catch ( const char* e )
    {
        ERR ( "%s", e );

    }

}


void agent::createControllersFromParsedAgent ( const unique_ptr<Parsed_Behavior>& behavior )
{
    for ( map<controller_name,controller_MapRules>::const_iterator it =behavior->controllers.begin(); it !=behavior->controllers.end(); ++it )
    {
        controller c ( it->second,behavior->inputs,symbol_table );
        controllers.push_back ( c );
    }
}


transitionTable agent::createAutomatonTableFromParsedAgent ( const unique_ptr<Parsed_Behavior>& behavior )
{
    transitionTable automaton_table_tmp;
    automaton_table_tmp.name = behavior->name;
    automaton_state s1, s2;
    transition e;
    for ( map<string,map<string,string> >::const_iterator it=behavior->automaton.begin(); it!=behavior->automaton.end(); ++it )
    {
        s1 = ( automaton_state ) map_discreteStateName_to_id.at ( it->first );
        for ( map<string,string>::const_iterator iit=it->second.begin(); iit!=it->second.end(); ++iit )
        {

            e= ( transition ) events_to_index.at ( iit->first );
            s2 = ( automaton_state ) map_discreteStateName_to_id.at ( iit->second );
            automaton_table_tmp.internalTable[s1][e]=s2;
        }
    }
    return automaton_table_tmp;
}


void agent::createDiscreteStateFromParsedAgent ( const unique_ptr<Parsed_Behavior>& behavior )
{
    automaton_state s =  0;
    unsigned int i = 0;
    for ( map<string,string>::const_iterator it=behavior->discrete_states.begin(); it!=behavior->discrete_states.end(); ++it )
    {
        map_discreteStateName_to_id.insert ( make_pair ( it->first,i ) );
        map_discreteStateId_to_controllerId.insert ( make_pair ( s,map_controllername_to_id.at ( it->second ) ) );
        inputs.commands[s];
        i++;
        s++;
    }
}


void agent::createSubEventsFromParsedAgent ( const unique_ptr<Parsed_Behavior>& behavior )
{

    unsigned i=0;
    for ( map<string,string>::const_iterator it=behavior->lambda_expressions.begin(); it!=behavior->lambda_expressions.end(); ++it )
    {
        sub_events_to_index.insert ( make_pair ( it->first,i ) );
        sub_events.insert ( make_pair ( i,Sub_events::_FALSE ) );
        i++;
    }

    for ( map<string,string>::const_iterator it=behavior->topology_expressions.begin(); it!=behavior->topology_expressions.end(); ++it )
    {
        sub_events_to_index.insert ( make_pair ( it->first,i ) );
        sub_events.insert ( make_pair ( i,Sub_events::_FALSE ) );
        i++;
    }
}


void agent::createEventsFromParsedAgent ( const unique_ptr<Parsed_Behavior>& behavior )
{
    transition i=0;
    for ( map<string,string>::const_iterator it=behavior->events_expressions.begin(); it!=behavior->events_expressions.end(); ++it )
    {
        events_to_index.insert ( make_pair ( it->first,i ) );
        events.insert ( make_pair ( i,Events::_FALSE ) );
        i++;
    }
}


void agent::createStateFromParsedAgent ( const unique_ptr<Parsed_Behavior>& behavior )
{
    for ( unsigned int i=0; i<behavior->state.size(); i++ )
    {
        state[i]=0;
        map_statename_to_id.insert ( std::pair<string,int> ( behavior->state.at ( i ),i ) );
    }
    for ( unsigned int i=0; i<state.size(); i++ )
    {
        symbol_table.add_variable ( behavior->state[i],state[i] );
    }

    for ( unsigned int i=0; i<behavior->inputs.size(); i++ )
    {
        inputs.default_command[i]=0;
        map_inputs_name_to_id.insert ( make_pair ( behavior->inputs.at ( i ),i ) );
    }
    for ( unsigned int i=0; i<inputs.default_command.size(); i++ )
    {
        symbol_table.add_variable ( behavior->inputs[i],inputs.default_command[i] );
    }
}




void agent::main_loop()
{

    // 		std::cout<<"time: "<<world_comm->receive_time()<<std::endl;
    const world_sim_packet& temp = world_comm->receive_agents_status();
    
        time=temp.time;

    for ( std::map<std::string,double>::const_iterator it=temp.bonus_variables.begin(); it!=temp.bonus_variables.end(); ++it )
    {
        bonusVariables.at ( map_bonus_variables_to_id.at ( it->first ) ) =it->second;
    }

    //TODO(Mirko): non e' ottimizzato, distrugge e ricrea ogni volta, follia!!
    for (auto object:objects.objects)
      delete object;
    objects.objects.clear();
     for ( auto it=temp.object_list.objects.begin(); it!=temp.object_list.objects.end(); ++it )
    {
      //(*it)->print(std::cout);
        objects.objects.push_back(*it);
// 	std::cout<<"task ricevuto:"<<it->second<<std::endl;
    }
   
    if (temp.state_agents.internal_map.find( identifier )==temp.state_agents.internal_map.end()){
       ERR("MANCA IL MIO STATO",NULL);
       throw("MANCA IL MIO STATO");
    }

    for ( auto it=temp.state_agents.internal_map.at ( identifier ).state.begin();it!=temp.state_agents.internal_map.at ( identifier ).state.end(); ++it )
    {
        state.at ( it->first ) =it->second;
    }

    sleep ( 0 );
    encoder->computeSubEvents ( temp.state_agents.internal_map );
    event_decoder.decode();

    /*!
     * Chiamo i plugin in fila
     */
    for ( unsigned int i=0; i<plugins.size(); i++ )
    {
        plugins[i]->run_plugin();
    }

    discreteState= automaton->getNextAutomatonState ( discreteState,events );

for ( auto discrete :discreteState )
    {
        controllers.at ( map_discreteStateId_to_controllerId.at ( discrete ) ).computeControl();
        inputs.commands.at ( discrete ) =inputs.default_command;
        //std::cout<< discrete;
    }
    //std::cout<< std::endl;
    world_comm->send_control_command ( inputs,identifier );

    for ( index_map::const_iterator it=map_discreteStateName_to_id.begin(); it!=map_discreteStateName_to_id.end(); ++it )
    {
        if ( it->second==discreteState.front() )
            cout<<it->first<<endl;
    }
}



agent::~agent()
{
    delete automaton;
    delete encoder;
    delete f_rndom;
    symbol_table.clear();
    for ( unsigned int i=0; i<plugins.size(); i++ )
    {
        delete ( plugins[i] );
    }
}
