#ifndef UDP_WORLD_COMMUNICATOR_H
#define UDP_WORLD_COMMUNICATOR_H


#include <async_udp_receiver.hpp>
#include "world_sniffer_abstract.h"
#include <boost/asio.hpp>

struct udp_target
{
	std::string identifier;
	std::string address;
};


class udp_world_sniffer: public world_sniffer_abstract
{
public:
    void start_receiving();
	udp_world_sniffer(std::vector<char>& buffer,	boost::asio::io_service& io_service);
// 	~udp_world_communicator();
private:
	boost::asio::io_service& io_service;
      async_udp_receiver world_sim_receiver;
	  world_sim_packet& infos;
	  std::vector<char> buffer;
	
	




	  
};

#endif // UDP_WORLD_COMMUNICATOR_H
