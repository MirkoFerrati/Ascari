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


class udp_world_communicator: public world_communicator_abstract
{
public:
    std::map<std::string,agent_state_packet> receive_agents_status();
	  std::map<std::string,double> receive_bonus_variables();
    simulation_time receive_time();
	void send_control_command(control_command_packet& command, const target_abstract* target);
	udp_world_communicator();
// 	~udp_world_communicator();
private:
	boost::asio::io_service io_service;
      udp_receiver<agents_name_to_states > agents_status_receiver;
      udp_receiver<std::map<std::string,double> > bonus_variables_receiver;
    
	udp_receiver<simulation_time> time_receiver;
	udp_sender<control_command_packet> control_command_sender;
	
};

#endif // UDP_WORLD_COMMUNICATOR_H
