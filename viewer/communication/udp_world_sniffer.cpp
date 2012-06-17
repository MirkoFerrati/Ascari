#include "udp_world_sniffer.h"

#include <boost/asio.hpp>



udp_world_sniffer::udp_world_sniffer(std::vector<char>& buffer,	boost::asio::io_service& io_service): 
io_service(io_service),infos(infos),
world_sim_receiver(io_service,
    boost::asio::ip::address::from_string("0.0.0.0"),
	boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT,buffer)
{
}




void udp_world_sniffer::start_receiving()
{
	world_sim_receiver.receive();
	
}



