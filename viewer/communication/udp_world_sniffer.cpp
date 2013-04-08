#include "udp_world_sniffer.h"

#include <boost/asio.hpp>



udp_world_sniffer::udp_world_sniffer(std::vector<char>& buffer,	boost::asio::io_service& io_service): 
io_service(io_service),
world_sim_receiver(io_service,
    boost::asio::ip::address::from_string(SOCKET_BINDING),
	boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT,buffer),infos(infos)
{
}




void udp_world_sniffer::start_receiving()
{
	world_sim_receiver.receive();
	
}

void udp_world_sniffer::stop_receiving()
{
	
	
}


