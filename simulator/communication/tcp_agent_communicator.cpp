#include "tcp_agent_communicator.h"

tcp_agent_communicator::tcp_agent_communicator(): 
	agent_communicator_abstract(),
time_sender(service,MULTICAST_ADDRESS,MULTICAST_PORT),
state_sender(service,MULTICAST_ADDRESS,MULTICAST_PORT)
{
	
}

std::vector< control_command_packet > tcp_agent_communicator::receive_control_commands()
{

}

void tcp_agent_communicator::send_broadcast(const simulation_time& time)
{
	time_sender.send(time);
}
void tcp_agent_communicator::send_broadcast(const std::vector< agent_state_packet >& infos)
{

}
void tcp_agent_communicator::send_target(const std::vector< agent_state_packet >& infos, const target_abstract* target)
{

}
