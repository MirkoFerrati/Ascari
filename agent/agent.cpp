#include "agent.h"
#include "communication/udp_world_communicator.h"
#include "logog.hpp"
#include "automaton/automatonFSM.h"

using namespace std;


agent::agent(std::string name,bool isDummy,const vector<Parsed_Agent>& agents)
:identifier(name),event_decoder(sub_events,events)
{
	int myAgent;
	for (unsigned int i =0;i<agents.size();i++)
            if (agents[i].name.compare(name)==0)
				myAgent=i;
	createStateFromParsedAgent(agents[myAgent]);
	createEventsFromParsedAgent(agents[myAgent]);
    world_comm=new udp_world_communicator();

    if (!isDummy)
    {
		automaton=new automatonFSM(createAutomatonTableFromParsedAgent(agents[myAgent]));
    }
    else {
        //TODO: we will think about identifierModule later
    }
	int i=0;
    for (map<controller_name,controller_MapRules>::const_iterator it =agents[myAgent].controllers.begin();it !=agents[myAgent].controllers.end();it++)
	{
		controller c(state,agents[i].state,it->second,agents[i].inputs,inputs);
		controllers.push_back(c);
		map_controllername_to_id.insert(make_pair(it->first,i++));
	}
	event_decoder.create(agents[myAgent].events_expressions,sub_events_to_index,events_to_index);
    main_loop();
}

transitionTable agent::createAutomatonTableFromParsedAgent(const Parsed_Agent& agent)
{
	transitionTable automaton_table_tmp;
	automaton_table_tmp.name = agent.automaton_name;
	automaton_state g1, g2;
	for (map<string,map<string,string> >::const_iterator it=agent.automaton.begin(); it!=agent.automaton.end();it++)
	{
		g1 = (automaton_state) map_discreteStateName_to_id.at(it->first);
		for (map<string,string>::const_iterator iit=it->second.begin();iit!=it->second.end();iit++){
		
// 		g2 = (automaton_state) map_discreteStateName_to_id.at(it->second);
// 		automaton_table_tmp.internalTable[id]; 
		}
	}
	//agent.automaton
	ERR("not implemented");
	throw "not implemented";
}

void agent::createEventsFromParsedAgent(const Parsed_Agent& agent)
{
<<<<<<< .mine
	transition i = (transition)0;
=======
	transition i=(transition)0;
>>>>>>> .r60
	for (map<string,string>::const_iterator it=agent.events_expressions.begin();it!=agent.events_expressions.end();it++)
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
		i++;
	}	
	for (unsigned int i=0;i<agent.inputs.size();i++)
	{	
		inputs[i]=0;
		map_inputs_name_to_id.insert(make_pair(agent.inputs.at(i),i));
		i++;
	}
	automaton_state s = (automaton_state) 0;
	unsigned int i = 0;
	for (map<string,string>::const_iterator it=agent.discrete_states.begin(); it!=agent.discrete_states.end(); it++)
	{	
		discreteState[i] = s;
		map_discreteStateName_to_id.insert(make_pair(it->first,i));
		map_discreteStateId_to_controllerId.insert(make_pair(s,map_controllername_to_id.at(it->second)));
		i++;
		s++;
	}
}

void agent::main_loop()
{

    try {
        while (1)
        {
// 		std::cout<<"time: "<<world_comm->receive_time()<<std::endl;
            sleep(1);
//		world_comm->send_control_command(dummy_control_packet,NULL);
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
}
