#ifndef UDP_WORLD_COMMUNICATOR_H
#define UDP_WORLD_COMMUNICATOR_H

#include "world_communicator_abstract.h"
#include <communication/udp_receiver.hpp>
#include <communication/udp_sender.hpp>


struct udp_target
{
	std::string identifier;
	std::string address;
};


class udp_world_communicator: public agent_namespace::world_communicator_abstract
{
public:
    agent_sim_packet_receiver receive_agents_status();
	void send_control_command(control_command_packet& command, const target_abstract& target);
	udp_world_communicator();
// 	~udp_world_communicator();
private:
	boost::asio::io_service io_service;
      udp_receiver<agent_sim_packet_receiver > world_sim_receiver;
    
	udp_receiver<simulation_time> time_receiver;
	udp_sender<control_command_packet> control_command_sender;
	
};

#endif // UDP_WORLD_COMMUNICATOR_H
