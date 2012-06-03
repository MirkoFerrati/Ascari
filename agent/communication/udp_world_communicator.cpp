#include "udp_world_communicator.h"

udp_world_communicator::udp_world_communicator(): 
 agents_status_receiver(io_service,
    boost::asio::ip::address::from_string("0.0.0.0"),
	boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT),
	time_receiver(io_service,
    boost::asio::ip::address::from_string("0.0.0.0"),
	boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT),
	control_command_sender(io_service,
	boost::asio::ip::address::from_string(MULTICAST_ADDRESS),SIMULATOR_PORT)
{
	
}

simulation_time udp_world_communicator::receive_time()
{
	return time_receiver.receive();
}

agents_name_to_states udp_world_communicator::receive_agents_status()
{
	return agents_status_receiver.receive();
}

void udp_world_communicator::send_control_command(control_command_packet& command, const target_abstract* target)
{
	control_command_sender.send(command);
}
