#include "agent.h"
#include "graph_routing/agent_router.h"

//written by Alessandro Settimi
#ifdef GLPK_FOUND
#include "task_assignment/task_assignment.h"
#endif
//written by Alessandro Settimi

#include "identifierModule/identifier_module.h"
#include <string>
#include <utility>
#include <map>
#include "communication/udp_world_communicator.h"
#include "communication/zmq_world_communicator.h"

#include "logog.hpp"
#include "automaton/automatonFSM.h"
#include "encoder/encoderDet.h"
#include "geometry.hpp"


using namespace std;

agent::agent(std::string name,const std::unique_ptr<Parsed_Behavior>& behavior, const Parsed_World & world)
        :identifier(name)
{
  
  createBonusVariablesFromWorld(world.bonus_expressions);
//qui poi uno setta tutto quello che manca oppure lascia vuoto
initialized=false;
encoder=0;
init(behavior,true);
}


agent::agent(int agent_index,const Parsed_World& world):
	identifier(world.agents.at(agent_index).name)
{
	createBonusVariablesFromWorld(world.bonus_expressions);
	
	if (!world.agents.at(agent_index).target_list.empty())
	{
		Plugin_module *plugin=new agent_router(world.agents.at(agent_index).target_list,events,events_to_index,world.agents.at(agent_index).name,time,world.graphName);
		plugins.push_back(plugin);
	}
	
	if (world.agents.at(agent_index).monitoring)
	{
		
		Plugin_module *monitor=new identifier_module(world,bonusVariables,map_bonus_variables_to_id,state_other_agents,time);
		plugins.push_back(monitor);
	}
	
	/*
	if (condition to enable a plugin)
	{
		Plugin_module *plugin=new empty_plugin(....);
		plugins.push_back(plugin);
	}
	*/
	
	#ifdef GLPK_FOUND
	//written by Alessandro Settimi
	if (!world.task_list.empty())
	{
		Plugin_module* plugin=new task_assignment(world,world.agents.at(agent_index),time,events,events_to_index);
		plugins.push_back(plugin);
	} 
	//written by Alessandro Settimi
	#endif

	/*!
	 * Aggiungo le variabili richieste dai plugin
	 */
	for (unsigned int i=0;i<plugins.size();i++)
	{
		plugins[i]->addReservedVariables(symbol_table);
		plugins[i]->addReservedVariables(encoder_symbol_table);
		
	}
	
	init(world.agents.at(agent_index).behavior,false);
	discreteState.push_front(map_discreteStateName_to_id.at(world.agents.at(agent_index).state_start));
	
//qui invece mi setto quello che manca a mano
}


void agent::init(const std::unique_ptr<Parsed_Behavior> & behavior, bool isDummy)
{
	time=0;//TODO(Mirko): initialize with the real time? Needs the agents to be synchronized with a common clock (now comes from the simulator)	
	symbol_table.add_constants();
	pi=exprtk::details::numeric::constant::pi;
	symbol_table.add_variable("PI_GRECO",pi,true);
	f_rndom = new rndom<double>();
	symbol_table.add_function(f_rndom->name, *f_rndom);
	
	int i=0;
	for (map<controller_name,controller_MapRules>::const_iterator it =behavior->controllers.begin();it !=behavior->controllers.end();++it)
	{
		map_controllername_to_id.insert(make_pair(it->first,i++));
	}
	event_decoder.setEventsAndSubEvents(&sub_events,&events);
	
	
	createDiscreteStateFromParsedAgent(behavior);
	createStateFromParsedAgent(behavior);
	createSubEventsFromParsedAgent(behavior);
	createEventsFromParsedAgent(behavior);
	
	
	createControllersFromParsedAgent(behavior);
	
	if (!isDummy)
	{
		automaton=new automatonFSM(createAutomatonTableFromParsedAgent(behavior));
		encoder=new encoderDet(sub_events, identifier,state,map_statename_to_id,bonusVariables,
							   map_bonus_variables_to_id, behavior->topology_expressions,
						 sub_events_to_index,behavior->lambda_expressions,encoder_symbol_table);
		//world_comm=new udp_world_communicator();
		world_comm=new zmq_world_communicator(identifier);
		 for (unsigned int i=0;i<plugins.size();i++)
	    {
		plugins[i]->compileExpressions(symbol_table);
	    }
	}
	else
	{
	   
		//TODO(Mirko): we will think about identifierModule later
		//TODO:create non-deterministic automaton
		automaton=new automatonFSM(createAutomatonTableFromParsedAgent(behavior));
		encoder=new encoderDet(sub_events, identifier,state,map_statename_to_id,bonusVariables,
							   map_bonus_variables_to_id, behavior->topology_expressions,
						 sub_events_to_index,behavior->lambda_expressions,encoder_symbol_table);
		 for (unsigned int i=0;i<plugins.size();i++)
	    {
		plugins[i]->compileExpressions(symbol_table);
	    }
	}
	
	event_decoder.create(behavior->events_expressions,sub_events_to_index,events_to_index);
	initialized=true;

}


void agent::createBonusVariablesFromWorld(map< bonusVariable, bonus_expression > bonus)
{
	int i=0;
	for (map<bonusVariable,bonus_expression>::const_iterator it=bonus.begin();it!=bonus.end();++it)
    {
		bonusVariables[i]=0;
        map_bonus_variables_to_id.insert(make_pair(it->first,i));
		i++;
    }
    for (index_map::const_iterator it=map_bonus_variables_to_id.begin();it!=map_bonus_variables_to_id.end();++it)
	{
		symbol_table.add_variable(it->first,bonusVariables[it->second]);
	}
	
}

void agent::start()
{
    if (!initialized)
        throw "please call init function before start";
    try {
        inputs.identifier = identifier;
        int cicli = 0;
	std::cout<<"inizio ciclo infinito di agent s_interrupted="<<s_interrupted<<std::endl;
        while (!s_interrupted) {
            agent_loop();
            cicli++;
            if (time < -1)
                break;
        }

    } catch (const char* e) {
        ERR ("%s", e);

    }

}


void agent::createControllersFromParsedAgent(const unique_ptr<Parsed_Behavior>& behavior)
{
    for (map<controller_name,controller_MapRules>::const_iterator it =behavior->controllers.begin();it !=behavior->controllers.end();++it)
    {
        controller c(it->second,behavior->inputs,symbol_table);
        controllers.push_back(c);
    }
}


transitionTable agent::createAutomatonTableFromParsedAgent(const unique_ptr<Parsed_Behavior>& behavior)
{
    transitionTable automaton_table_tmp;
    automaton_table_tmp.name = behavior->name;
    automaton_state s1, s2;
    transition e;
    for (map<string,map<string,string> >::const_iterator it=behavior->automaton.begin(); it!=behavior->automaton.end();++it)
    {
        s1 = (automaton_state) map_discreteStateName_to_id.at(it->first);
        for (map<string,string>::const_iterator iit=it->second.begin();iit!=it->second.end();++iit) {

            e= (transition) events_to_index.at(iit->first);
            s2 = (automaton_state) map_discreteStateName_to_id.at(iit->second);
            automaton_table_tmp.internalTable[s1][e]=s2;
        }
    }
    return automaton_table_tmp;
}


void agent::createDiscreteStateFromParsedAgent(const unique_ptr<Parsed_Behavior>& behavior)
{
    automaton_state s =  0;
    unsigned int i = 0;
    for (map<string,string>::const_iterator it=behavior->discrete_states.begin(); it!=behavior->discrete_states.end(); ++it)
    {
        map_discreteStateName_to_id.insert(make_pair(it->first,i));
        map_discreteStateId_to_controllerId.insert(make_pair(s,map_controllername_to_id.at(it->second)));
	inputs.commands[s];
        i++;
        s++;
    }
}


void agent::createSubEventsFromParsedAgent(const unique_ptr<Parsed_Behavior>& behavior) {

    unsigned i=0;
    for (map<string,string>::const_iterator it=behavior->lambda_expressions.begin();it!=behavior->lambda_expressions.end();++it) {
        sub_events_to_index.insert(make_pair(it->first,i));
        sub_events.insert(make_pair(i,Sub_events::_FALSE));
        i++;
    }

    for (map<string,string>::const_iterator it=behavior->topology_expressions.begin();it!=behavior->topology_expressions.end();++it) {
        sub_events_to_index.insert(make_pair(it->first,i));
        sub_events.insert(make_pair(i,Sub_events::_FALSE));
		i++;
    }
}


void agent::createEventsFromParsedAgent(const unique_ptr<Parsed_Behavior>& behavior)
{
    transition i=0;
    for (map<string,string>::const_iterator it=behavior->events_expressions.begin();it!=behavior->events_expressions.end();++it)
    {
        events_to_index.insert(make_pair(it->first,i));
        events.insert(make_pair(i,Events::_FALSE));
        i++;
    }
}


void agent::createStateFromParsedAgent(const unique_ptr<Parsed_Behavior>& behavior)
{
    for (unsigned int i=0;i<behavior->state.size();i++)
    {
        state[i]=0;
        map_statename_to_id.insert(std::pair<string,int>(behavior->state.at(i),i));
    }
    for (unsigned int i=0;i<state.size();i++)
	{
		symbol_table.add_variable(behavior->state[i],state[i]);
	}
	
    for (unsigned int i=0;i<behavior->inputs.size();i++)
    {
        inputs.default_command[i]=0;
        map_inputs_name_to_id.insert(make_pair(behavior->inputs.at(i),i));
    }
    for (unsigned int i=0;i<inputs.default_command.size();i++)
	{
		symbol_table.add_variable(behavior->inputs[i],inputs.default_command[i]);
	}
}




void agent::agent_loop()
{
        {
	// 		std::cout<<"time: "<<world_comm->receive_time()<<std::endl;
			world_sim_packet temp=world_comm->receive_agents_status();
			state_other_agents.swap(temp.state_agents.internal_map);//TODO(Mirko): si possono evitare le copie e gli swap?
			time=temp.time;
			
			for (std::map<std::string,double>::const_iterator it=temp.bonus_variables.begin();it!=temp.bonus_variables.end();++it)
			{
			bonusVariables.at(map_bonus_variables_to_id.at(it->first))=it->second; 
			}
           
	
			main_loop();
        }



}


void agent::dummy_loop(const std::map<int,double> & sensed_bonus_variables,const std::map<std::string,agent_state_packet> &sensed_state_agents,const simulation_time & sensed_time)
    
{
        {
			state_other_agents.insert(sensed_state_agents.begin(),sensed_state_agents.end());//TODO(Mirko): si possono evitare le copie e gli swap?
			time=sensed_time;
			
			bonusVariables.insert(sensed_bonus_variables.begin(),sensed_bonus_variables.end()); 
			
	
			main_loop();
        }



}



void agent::main_loop()
{
        		//TODO(Mirko): questo ciclo for copia informazioni che in teoria gia' abbiamo, forse non vale la pena di usare la variabile state
            for (map<int,double>::const_iterator it=state_other_agents.at(identifier).state.begin();
                    it!=state_other_agents.at(identifier).state.end();++it)
            {
                state.at(it->first)=it->second;
            }

            cout<<"stato: "<<state.at(0)<<" "<<state.at(1)<<" "<<state.at(2)<<endl;

            sleep(0);
            encoder->computeSubEvents(state_other_agents);
            event_decoder.decode();
			
			/*!
			 * Chiamo i plugin in fila
			 */
			for (unsigned int i=0;i<plugins.size();i++)
			{
				plugins[i]->run_plugin();
			}
			
            discreteState= automaton->getNextAutomatonState(discreteState,events);
	    
	    for ( auto discrete :discreteState)
	    {
	      controllers.at(map_discreteStateId_to_controllerId.at(discrete)).computeControl();
	      inputs.commands.at(discrete)=inputs.default_command;
	    }
            world_comm->send_control_command(inputs,NULL);

            string tmp; //TODO(Mirko) non ha senso inizializzare una stringa ad ogni giro solo per stampare lo stato dell'agente, trovare un metodo migliore
            for (index_map::const_iterator it=map_discreteStateName_to_id.begin();it!=map_discreteStateName_to_id.end();++it)
            {
                if (it->second==discreteState.front())
                    tmp=it->first;
            }
            cout<<tmp<<endl;

//             cout<<tmp<<" "<<state_other_agents.at(identifier).state.at(0)<<" "<<state_other_agents.at(identifier).state.at(1)
// 			<<" "<<state_other_agents.at(identifier).state.at(2)<<endl;

// 		if (abs(state_other_agents.at(identifier).state.at(0))>=29.99)
// 			break;

        



}



agent::~agent()
{
    delete world_comm;
    delete automaton;
    delete encoder;
    delete f_rndom;
    for (unsigned int i=0;i<plugins.size();i++)
			{
				delete(plugins[i]);
			}
}
