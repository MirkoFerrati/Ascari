#include "agent.h"
#include <agent_router.h>
#include <string>
#include <utility>
#include <map>
#include "communication/udp_world_communicator.h"

#include "logog.hpp"
#include "automaton/automatonFSM.h"
#include "encoder/encoderDet.h"
#include "geometry.hpp"


using namespace std;


agent::agent(std::string name,bool isDummy,const Parsed_World& world)
        :identifier(name),event_decoder(sub_events,events)
{
    int myAgent=-1;
    for (unsigned int i =0;i<world.agents.size();i++)
        if (world.agents.at(i).name.compare(name)==0)
            myAgent=i;
	if (myAgent==-1)
	{
		ERR("agent %s not found in configuration file, please check for agent names",name.c_str());
		throw "agent name not found in configuration file, please check for agent names";
	}
	string temp=name;
	time=0;//TODO(Mirko): initialize with the real time? Needs the agents to be synchronized with a common clock (now comes from the simulator)	
    
    symbol_table.add_constants();
	pi=exprtk::details::numeric::constant::pi;
	symbol_table.add_variable("PI_GRECO",pi,true);
	f_rndom = new rndom<double>();
	symbol_table.add_function(f_rndom->name, *f_rndom);
    
    int i=0;
    for (map<controller_name,controller_MapRules>::const_iterator it =world.agents[myAgent].controllers.begin();it !=world.agents[myAgent].controllers.end();++it)
    {
        map_controllername_to_id.insert(make_pair(it->first,i++));
    }
    createBonusVariablesFromWorld(world.bonus_expressions);
    createDiscreteStateFromParsedAgent(world.agents.at(myAgent));
    createStateFromParsedAgent(world.agents.at(myAgent));
    createSubEventsFromParsedAgent(world.agents.at(myAgent));
    createEventsFromParsedAgent(world.agents.at(myAgent));
    world_comm=new udp_world_communicator();
	
	if (!world.agents.at(myAgent).target_list.empty())
	{
		Plugin_module *plugin=new agent_router(world.agents.at(myAgent).target_list,events,events_to_index,temp,time,world.graphName);;
		plugins.push_back(plugin);
	}
	  
	/*!
	 * Aggiungo le variabili richieste dai plugin
	 */
	for (unsigned int i=0;i<plugins.size();i++)
	{
		plugins[i]->addReservedVariables(symbol_table);
		plugins[i]->addReservedVariables(encoder_symbol_table);
		plugins[i]->compileExpressions(symbol_table);
	}
		
	createControllersFromParsedAgent(world.agents.at(myAgent));

    if (!isDummy)
    {
        automaton=new automatonFSM(createAutomatonTableFromParsedAgent(world.agents.at(myAgent)));
        encoder=new encoderDet(sub_events, identifier,state,map_statename_to_id,bonusVariables,
                               map_bonus_variables_to_id, world.agents.at(myAgent).topology_expressions,
							   sub_events_to_index,world.agents.at(myAgent).lambda_expressions,encoder_symbol_table);
    }
    else
	{
        //TODO(Mirko): we will think about identifierModule later
    }

    event_decoder.create(world.agents[myAgent].events_expressions,sub_events_to_index,events_to_index);
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
	main_loop();
}


void agent::createControllersFromParsedAgent(const Parsed_Agent& agent)
{
    for (map<controller_name,controller_MapRules>::const_iterator it =agent.controllers.begin();it !=agent.controllers.end();++it)
    {
        controller c(it->second,agent.inputs,symbol_table);
        controllers.push_back(c);
    }
}


transitionTable agent::createAutomatonTableFromParsedAgent(const Parsed_Agent& agent)
{
    transitionTable automaton_table_tmp;
    automaton_table_tmp.name = agent.automaton_name;
    automaton_state s1, s2;
    transition e;
    for (map<string,map<string,string> >::const_iterator it=agent.automaton.begin(); it!=agent.automaton.end();++it)
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


void agent::createDiscreteStateFromParsedAgent(const Parsed_Agent& agent)
{
    automaton_state s = (automaton_state) 0;
    unsigned int i = 0;
    for (map<string,string>::const_iterator it=agent.discrete_states.begin(); it!=agent.discrete_states.end(); ++it)
    {
        map_discreteStateName_to_id.insert(make_pair(it->first,i));
        map_discreteStateId_to_controllerId.insert(make_pair(s,map_controllername_to_id.at(it->second)));
        i++;
        s++;
    }
    discreteState.push_back(map_discreteStateName_to_id.at(agent.state_start));
}


void agent::createSubEventsFromParsedAgent(const Parsed_Agent& agent) {

    unsigned i=0;
    for (map<string,string>::const_iterator it=agent.lambda_expressions.begin();it!=agent.lambda_expressions.end();++it) {
        sub_events_to_index.insert(make_pair(it->first,i));
        sub_events.insert(make_pair(i,_FALSE));
        i++;
    }

    for (map<string,string>::const_iterator it=agent.topology_expressions.begin();it!=agent.topology_expressions.end();++it) {
        sub_events_to_index.insert(make_pair(it->first,i));
        sub_events.insert(make_pair(i,_FALSE));
		i++;
    }
}


void agent::createEventsFromParsedAgent(const Parsed_Agent& agent)
{
    transition i=(transition)0;
    for (map<string,string>::const_iterator it=agent.events_expressions.begin();it!=agent.events_expressions.end();++it)
    {
        events_to_index.insert(make_pair(it->first,i));
        events.insert(make_pair(i,false));
        i++;
    }
}


void agent::createStateFromParsedAgent(const Parsed_Agent& agent)
{
    for (unsigned int i=0;i<agent.state.size();i++)
    {
        state[i]=0;
        map_statename_to_id.insert(std::pair<string,int>(agent.state.at(i),i));
    }
    for (unsigned int i=0;i<state.size();i++)
	{
		symbol_table.add_variable(agent.state[i],state[i]);
	}
	
    for (unsigned int i=0;i<agent.inputs.size();i++)
    {
        inputs.command[i]=0;
        map_inputs_name_to_id.insert(make_pair(agent.inputs.at(i),i));
    }
    for (unsigned int i=0;i<inputs.command.size();i++)
	{
		symbol_table.add_variable(agent.inputs[i],inputs.command[i]);
	}
}




void agent::main_loop()
{

    try {
        inputs.identifier=identifier;
        int cicli=0;
        while (1)
        {
            cicli++;
	// 		std::cout<<"time: "<<world_comm->receive_time()<<std::endl;
			world_sim_packet temp=world_comm->receive_agents_status();
			state_other_agents.swap(temp.state_agents.internal_map);
			time=temp.time;
			if (time<-1)
				break;
			for (std::map<std::string,double>::const_iterator it=temp.bonus_variables.begin();it!=temp.bonus_variables.end();++it)
			{
			bonusVariables.at(map_bonus_variables_to_id.at(it->first))=it->second; 
			}
           
			//TODO(Mirko): questo ciclo for copia informazioni che in teoria gia' abbiamo, forse non vale la pena di usare la variabile state
            for (map<int,double>::const_iterator it=state_other_agents.at(identifier).state.begin();
                    it!=state_other_agents.at(identifier).state.end();++it)
            {
                state.at(it->first)=it->second;
            }
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
            controllers.at(map_discreteStateId_to_controllerId.at(discreteState.at(0))).computeControl();

            world_comm->send_control_command(inputs,NULL);

            //string tmp; //TODO(Mirko) non ha senso inizializzare una stringa ad ogni giro solo per stampare lo stato dell'agente, trovare un metodo migliore
            //for (index_map::const_iterator it=map_discreteStateName_to_id.begin();it!=map_discreteStateName_to_id.end();++it)
            //{
            //    if (it->second==discreteState[0])
            //        tmp=it->first;
            //}

//             cout<<tmp<<" "<<state_other_agents.at(identifier).state.at(0)<<" "<<state_other_agents.at(identifier).state.at(1)
// 			<<" "<<state_other_agents.at(identifier).state.at(2)<<endl;

// 		if (abs(state_other_agents.at(identifier).state.at(0))>=29.99)
// 			break;

        }

    }
    catch (const char* e)
    {
        std::cerr<<e<<std::endl;

    }

}

agent::~agent()
{
    delete world_comm;
    delete automaton;
    delete encoder;
	delete f_rndom;
}
