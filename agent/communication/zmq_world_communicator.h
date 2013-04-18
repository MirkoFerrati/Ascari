#ifndef ZMQ_WORLD_COMMUNICATOR_H
#define ZMQ_WORLD_COMMUNICATOR_H
#include "world_communicator_abstract.h"
#include "../shared/communication/zmq_full_communicator.hpp"
#include <types/agent_sim_packet.h>


class zmq_world_communicator : public agent_namespace::world_communicator_abstract,
public zmq_agent_to_simulator_communicator<agent_sim_packet_receiver,control_command_packet>
{
public:
zmq_world_communicator(std::string agent_name);
const world_sim_packet& receive_agents_status();
void send_control_command( control_command_packet& packet, const target_abstract& );
inline const world_sim_packet& get_last_received()
{
  return packet_received;
  
}
~zmq_world_communicator();

private:
world_sim_packet packet_received;

};


#endif //ZMQ_WORLD_COMMUNICATOR_H