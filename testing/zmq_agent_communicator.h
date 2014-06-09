#ifndef ZMQ_AGENT_COMMUNICATOR_H
#define ZMQ_AGENT_COMMUNICATOR_H
#include <communication/zmq_full_communicator.hpp>
#include <types/control_command_packet.h>
#include <types/agent_sim_packet.h>
#include <zmq.hpp>

class zmq_agent_communicator: 
public zmq_simulator_to_agent_communicator<control_command_packet,agent_sim_packet>
{
public:
zmq_agent_communicator(unsigned int expected_senders);
zmq_agent_communicator(unsigned int expected_senders, std::list<std::string>clients);
    std::vector< control_command_packet >& receive_control_commands();
void send_broadcast(const agent_sim_packet& infos);
void send_target(const agent_sim_packet& infos, const target_abstract& target);
~zmq_agent_communicator();	

};



#endif //ZMQ_AGENT_COMMUNICATOR_H