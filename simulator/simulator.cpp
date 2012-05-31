#include "simulator.h"
#include <tcp_agent_communicator.h>
#include "typedefs.h"

void simulator::create_communicator(int communicator_type)
{
	if (communicator_type==1)
	{
		communicator=new tcp_agent_communicator();
	}
}


void simulator::main_loop()
{

}

simulator::simulator()
{

}

void simulator::start_sim()
{

}
