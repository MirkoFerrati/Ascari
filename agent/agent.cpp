#include "agent.h"
#include "communication/udp_world_communicator.h"
#include "logog.hpp"

agent::agent(std::string name):identifier(name)
{
	world_comm=new udp_world_communicator();
	main_loop();
}

void agent::main_loop()
{
	control_command_packet dummy_control_packet;
	dummy_control_packet.identifier=identifier;
	dummy_control_packet.command.insert(1,1);
	try{
	
	int i=0;
	while(1)
	{
		std::cout<<"time: "<<world_comm->receive_time()<<std::endl;
		dummy_control_packet.command[1]=identifier.c_str()[1]/10;
		sleep(1);
		world_comm->send_control_command(dummy_control_packet,NULL);
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
