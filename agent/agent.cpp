#include "agent.h"

#include <string>
#include <utility>
#include <map>
#include "communication/zmq_world_communicator.h"
#include "communication/zmq_real_world_serial_communicator.h"


#include "logog.hpp"

using namespace std;

agent::agent ( std::string name,const std::unique_ptr<Parsed_Behavior>& behavior, const Parsed_World & world )
    :identifier ( name ),behavior(behavior),world(world),noStart(false)
{
    initialized=false;
}

agent::agent ( const Parsed_World& world, bool noStart ) :
    identifier ( world.agents.front().name ),behavior(world.agents.front().behavior),world(world),noStart(noStart)
{
}

void agent::initialize()
{
      createBonusVariablesFromWorld ( world.bonus_expressions );
    
    init(behavior,noStart);
      
     for ( unsigned int i=0; i<plugins.size(); i++ )
    {
        plugins[i]->initialize();
    } 
      
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
    
    int i=0;

    createStateFromParsedAgent ( behavior );

	if ( !noStart ) 
        {
			if ( world.agents.front().simulated )
				world_comm=std::make_shared<zmq_world_communicator> ( identifier );
			else{
				world_comm=std::make_shared<zmq_real_world_serial_communicator> ( identifier,map_inputs_name_to_id );
				//world_comm=std::make_shared<zmq_world_communicator> ( identifier );
			}
		}
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

}

void agent::start()
{
    if ( !initialized )
        throw "please call init function before start";
    try
    {
        inputs.identifier = identifier;
        int cicli = 0;
        std::cout<<"agent main infinite loop started"<<std::endl;
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




void agent::createStateFromParsedAgent ( const unique_ptr<Parsed_Behavior>& behavior )
{
    for ( unsigned int i=0; i<behavior->state.size(); i++ )
    {
        state[i]=0;
        map_statename_to_id.insert ( std::pair<string,int> ( behavior->state.at ( i ),i ) );
    }

    for ( unsigned int i=0; i<behavior->inputs.size(); i++ )
    {
        inputs.command[i]=0;
        map_inputs_name_to_id.insert ( make_pair ( behavior->inputs.at ( i ),i ) );
    }
}




void agent::main_loop()
{
    const world_sim_packet& temp = world_comm->receive_agents_status();
    time=temp.time;

    for ( std::map<std::string,double>::const_iterator it=temp.bonus_variables.begin(); it!=temp.bonus_variables.end(); ++it )
    {
        bonusVariables.at ( map_bonus_variables_to_id.at ( it->first ) ) =it->second;
    }

    //TODO(Mirko): non e' ottimizzato, distrugge e ricrea ogni volta, follia!!    
    for ( auto it=objects.objects.begin(); it!=objects.objects.end(); ++it )
    {
	for( auto inner_it:it->second) delete inner_it;
    }
    for ( auto it=temp.object_list.objects.begin(); it!=temp.object_list.objects.end(); ++it )
    {
        objects.objects[it->first]=it->second;
    }
   
    if (temp.state_agents.internal_map.find( identifier )==temp.state_agents.internal_map.end()){
       ERR("MANCA IL MIO STATO",NULL);
       throw("MANCA IL MIO STATO");
    }

    for ( auto it=temp.state_agents.internal_map.at ( identifier ).state.begin();it!=temp.state_agents.internal_map.at ( identifier ).state.end(); ++it )
    {
        state.at ( it->first ) =it->second;
	//cout<<it->first<<":"<<it->second<<endl;
    }

    sleep ( 0 );

    /*!
     * Chiamo i plugin in fila
     */
    for ( unsigned int i=0; i<plugins.size(); i++ )
    {
        plugins[i]->run_plugin();
    }
    //std::cout<< std::endl;
    world_comm->send_control_command ( inputs,identifier );

}



agent::~agent()
{
    delete f_rndom;
    for ( unsigned int i=0; i<plugins.size(); i++ )
    {
        delete ( plugins[i] );
    }
}
