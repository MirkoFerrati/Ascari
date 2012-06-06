#include "agent.h"
#include "communication/udp_world_communicator.h"
#include "logog.hpp"
#include "automaton/automatonFSM.h"




agent::agent(std::string name,bool isDummy,const vector<Parsed_Agent>& agents):identifier(name)
{
	world_comm=new udp_world_communicator();
	
	if (!isDummy)
	{
		for (int i =0;i<agents.size();i++)
			if (agents[i].name.compare(name)==0)
				automaton=new automatonFSM(createAutomatonTableFromParsedAgent(agents[i]));
	}
	else{
		//TODO: we will think about identifierModule later
	}
	
	
	
	main_loop();
}

transitionTable agent::createAutomatonTableFromParsedAgent(const Parsed_Agent& agent)
{
	
}


void agent::main_loop()
{
  
	try{
	
	int i=0;
	while(1)
	{
		std::cout<<"time: "<<world_comm->receive_time()<<std::endl;
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
