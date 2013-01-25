#include "zmq_world_communicator.h"


zmq_world_communicator::zmq_world_communicator(std::string agent_name)
{
	init(agent_name);
}


world_sim_packet zmq_world_communicator::receive_agents_status()
{
return receive().front(); //TODO(Mirko): zerocopy here
}


void zmq_world_communicator::send_control_command(control_command_packet& packet, const target_abstract* /*target*/)
{
send(packet);
}


zmq_world_communicator::~zmq_world_communicator()
{

}
