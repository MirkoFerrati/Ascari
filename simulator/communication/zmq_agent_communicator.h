#ifndef ZMQ_AGENT_COMMUNICATOR_H
#define ZMQ_AGENT_COMMUNICATOR_H
#include "../communication/agent_communicator_abstract.h"
#include <communication/zmq_full_communicator.hpp>
#include <zmq.hpp>

class zmq_agent_communicator : public simulator_namespace::agent_communicator_abstract,
public zmq_simulator_to_agent_communicator<control_command_packet,world_sim_packet>
{
public:
zmq_agent_communicator(unsigned int expected_senders);
zmq_agent_communicator(unsigned int expected_senders, std::list<std::string>clients);
std::vector< control_command_packet > receive_control_commands();
void send_broadcast(const world_sim_packet& infos);
void send_target(const world_sim_packet& infos, const target_abstract* target);
~zmq_agent_communicator();	

};



#endif //ZMQ_AGENT_COMMUNICATOR_H