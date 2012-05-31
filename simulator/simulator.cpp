#include "simulator.h"
#include <udp_agent_communicator.h>
#include "typedefs.h"

void simulator::create_communicator(int communicator_type)
{
	if (communicator_type==1)
	{
		communicator=new udp_agent_communicator();
	}
}


void simulator::main_loop()
{
	commands=communicator->receive_control_commands();
	communicator->send_broadcast(states_index);
	dynamic_module.evolve(states_index,commands);
}

simulator::simulator()
{

}

void simulator::start_sim()
{
	simulation_time time=0;
	communicator->send_broadcast(time);
	main_loop();
}
