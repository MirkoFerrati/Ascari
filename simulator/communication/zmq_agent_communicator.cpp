#include "zmq_agent_communicator.h"

zmq_agent_communicator::zmq_agent_communicator(unsigned int expected_senders, std::list<std::string> clients)
{
	setClientsName(clients);
	init(expected_senders);
}

zmq_agent_communicator::zmq_agent_communicator(unsigned int expected_senders)
{
	init(expected_senders);
}




std::vector< control_command_packet > zmq_agent_communicator::receive_control_commands()
{
	return receive();
}

void zmq_agent_communicator::send_broadcast(const world_sim_packet& infos)
{
	send(infos);
}


void zmq_agent_communicator::send_target(const world_sim_packet& infos, const target_abstract& target)
{
    send(infos,target);
}


zmq_agent_communicator::~zmq_agent_communicator()
{
	
}

