#ifndef TCP_AGENT_COMMUNICATOR_H
#define TCP_AGENT_COMMUNICATOR_H
#include <agent_communicator_abstract.h>
#include "tcp_sender.hpp"
#include "tcp_receiver.hpp"

class tcp_agent_communicator: public agent_communicator_abstract
{
public:
	tcp_agent_communicator();
    std::vector< control_command_packet > receive_control_commands();
    void send_broadcast(const std::vector< agent_state_packet >& infos);
    void send_broadcast(const simulation_time& time );
    void send_target(const std::vector< agent_state_packet >& infos, const target_abstract* target);
private:
	boost::asio::io_service service;//This must be written before any sender or receiver
	tcp_sender<const std::vector<agent_state_packet> > state_sender;
	tcp_receiver< const std::vector<control_command_packet> > control_receiver;
	tcp_sender<simulation_time> time_sender;
};

#endif // TCP_AGENT_COMMUNICATOR_H
