#include "udp_agent_communicator.h"

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
	control_receiver.receive();
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
