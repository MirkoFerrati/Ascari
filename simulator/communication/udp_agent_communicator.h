#ifndef TCP_AGENT_COMMUNICATOR_H
#define TCP_AGENT_COMMUNICATOR_H
#include <agent_communicator_abstract.h>
#include "communication/udp_sender.hpp"
#include "communication/udp_receiver.hpp"


class udp_agent_communicator: public agent_communicator_abstract
{
public:
	udp_agent_communicator();
    std::vector< control_command_packet > receive_control_commands();
    void send_broadcast(const agents_name_to_states& infos);
    void send_broadcast(const simulation_time& time );
    void send_target(const agents_name_to_states&  infos, const target_abstract* target);
private:
	boost::asio::io_service service;//This must be written before any sender or receiver
	udp_sender<simulation_time> time_sender;
	udp_sender<agents_name_to_states> state_sender;
	udp_receiver<control_command_packet > control_receiver;
	
};

#endif // TCP_AGENT_COMMUNICATOR_H
