#include "agent.h"
#include "communication/udp_world_communicator.h"

agent::agent()
{
	world_comm=new udp_world_communicator();
	main_loop();
}

void agent::main_loop()
{
	while(1)
	{
		
	}
}
