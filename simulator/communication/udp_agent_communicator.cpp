#include "udp_agent_communicator.h"

#include "typedefs.h"

udp_agent_communicator::udp_agent_communicator():
        agent_communicator_abstract(),
        time_sender(service, boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT),
        state_sender(service, boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT),
        control_receiver(service, boost::asio::ip::address::from_string("0.0.0.0"),
						 boost::asio::ip::address::from_string(MULTICAST_ADDRESS),SIMULATOR_PORT)
{

}

std::vector< control_command_packet > udp_agent_communicator::receive_control_commands()
{
	std::vector< control_command_packet> results;
	for (int i=0;i<NUM_AGENTS;i++)
		results.push_back(control_receiver.receive());
	return results;
}

void udp_agent_communicator::send_broadcast(const simulation_time& time)
{
    time_sender.send(time);
}
void udp_agent_communicator::send_broadcast(const agents_name_to_states& infos)
{
	state_sender.send(infos);
}
void udp_agent_communicator::send_target(const agents_name_to_states&  infos, const target_abstract* target)
{

}
