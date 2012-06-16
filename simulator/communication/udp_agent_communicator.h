#ifndef TCP_AGENT_COMMUNICATOR_H
#define TCP_AGENT_COMMUNICATOR_H
#include <agent_communicator_abstract.h>
#include "communication/udp_sender.hpp"
#include "communication/udp_receiver.hpp"


class udp_agent_communicator: public agent_communicator_abstract
{
public:
	udp_agent_communicator(int num_agents);
    std::vector< control_command_packet > receive_control_commands();
    void send_broadcast(const simulation_time& time );
    virtual void send_broadcast(const world_sim_packet& infos);
    void send_target(const world_sim_packet&  infos, const target_abstract* target);
private:
	boost::asio::io_service service;//This must be written before any sender or receiver
	udp_sender<simulation_time> time_sender;
	udp_sender<world_sim_packet> state_sender;
	udp_receiver<control_command_packet > control_receiver;
	int num_agents;
};

#endif // TCP_AGENT_COMMUNICATOR_H
