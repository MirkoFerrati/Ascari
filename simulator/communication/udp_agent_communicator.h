#ifndef TCP_AGENT_COMMUNICATOR_H
#define TCP_AGENT_COMMUNICATOR_H
#include "communication/udp_sender.hpp"
#include "communication/udp_receiver.hpp"
#include "../communication/agent_communicator_abstract.h"


class udp_agent_communicator: public simulator_namespace::agent_communicator_abstract
{
public:
	udp_agent_communicator(int num_agents);
    std::vector< control_command_packet >& receive_control_commands();
    void send_broadcast(const simulation_time& time );
    virtual void send_broadcast(const agent_sim_packet& infos);
    void send_target(const agent_sim_packet&  infos, const target_abstract& target);
    ~udp_agent_communicator();
private:
	unsigned int num_agents;
	boost::asio::io_service service;//This must be written before any sender or receiver
	udp_sender<agent_sim_packet> state_sender;
	udp_receiver<control_command_packet > control_receiver;
	
};

#endif // TCP_AGENT_COMMUNICATOR_H
