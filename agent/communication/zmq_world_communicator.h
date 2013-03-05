#ifndef ZMQ_WORLD_COMMUNICATOR_H
#define ZMQ_WORLD_COMMUNICATOR_H
#include "world_communicator_abstract.h"
#include "../shared/communication/zmq_full_communicator.hpp"


class zmq_world_communicator : public agent_namespace::world_communicator_abstract,
public zmq_agent_to_simulator_communicator<agent_sim_packet_receiver,control_command_packet>
{
public:
zmq_world_communicator(std::string agent_name);
agent_sim_packet_receiver receive_agents_status();
void send_control_command( control_command_packet& packet, const target_abstract& );
~zmq_world_communicator();

private:


};


#endif //ZMQ_WORLD_COMMUNICATOR_H