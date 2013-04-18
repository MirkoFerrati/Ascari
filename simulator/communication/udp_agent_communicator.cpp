#include "udp_agent_communicator.h"



udp_agent_communicator::udp_agent_communicator(int num_agents):
        agent_communicator_abstract(),num_agents(num_agents),
        state_sender(service, boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT),
        
        control_receiver(service, boost::asio::ip::address::from_string(SOCKET_BINDING),
						 boost::asio::ip::address::from_string(MULTICAST_ADDRESS),SIMULATOR_PORT)
{

}

std::vector< control_command_packet > udp_agent_communicator::receive_control_commands()
{
	std::vector< control_command_packet> results;
	for (unsigned int i=0;i<num_agents;i++)
		results.push_back(control_receiver.receive());
	return results;
}


void udp_agent_communicator::send_broadcast(const agent_sim_packet& infos)
{
	state_sender.send(infos);
}


void udp_agent_communicator::send_target(const agent_sim_packet&  infos, const target_abstract& /*target*/)
{

}

udp_agent_communicator::~udp_agent_communicator()
{

}
