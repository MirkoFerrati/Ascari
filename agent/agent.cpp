#include "agent.h"
#include "communication/udp_world_communicator.h"
#include "logog.hpp"
#include "automaton/automatonFSM.h"




agent::agent(std::string name,bool isDummy,const vector<Parsed_Agent>& agents):identifier(name)
{
	int myAgent;
	for (int i =0;i<agents.size();i++)
            if (agents[i].name.compare(name)==0)
				myAgent=i;
	
	for (int i=0;i<agents[myAgent].state.size();i++)
	{	
		state[i]=0;
		map_statename_to_id.insert(make_pair<string,int>(agents[myAgent].state.at(i),i));
		i++;
	}	
	for (int i=0;i<agents[myAgent].inputs.size();i++)
	{	
		inputs[i]=0;
		map_inputs_name_to_id.insert(make_pair<string,int>(agents[myAgent].inputs.at(i),i));
		i++;
	}	
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
		map_controllername_to_id.insert(make_pair<string,int>(it->first,i++));
	}
    main_loop();
}

transitionTable agent::createAutomatonTableFromParsedAgent(const Parsed_Agent& agent)
{

}


void agent::main_loop()
{

    try {

        int i=0;
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
