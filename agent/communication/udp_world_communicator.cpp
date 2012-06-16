#include "udp_world_communicator.h"

udp_world_communicator::udp_world_communicator(): 
 world_sim_receiver(io_service,
    boost::asio::ip::address::from_string("0.0.0.0"),
	boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT),
	time_receiver(io_service,
    boost::asio::ip::address::from_string("0.0.0.0"),
	boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT),
	control_command_sender(io_service,
	boost::asio::ip::address::from_string(MULTICAST_ADDRESS),SIMULATOR_PORT)
{
	
}
/*
udp_world_communicator::~udp_world_communicator()
{
	io_service.~io_service();
}*/



simulation_time udp_world_communicator::receive_time()
{
	return time_receiver.receive();
}

world_sim_packet udp_world_communicator::receive_agents_status()
{
	return world_sim_receiver.receive();
}



void udp_world_communicator::send_control_command(control_command_packet& command, const target_abstract* target)
{
	control_command_sender.send(command);
}
