#ifndef ZMQ_REAL_WORLD_SERIAL_COMMUNICATOR_H
#define ZMQ_REAL_WORLD_SERIAL_COMMUNICATOR_H
#include "world_communicator_abstract.h"
#include "../shared/communication/zmq_full_communicator.hpp"
#include <types/agent_sim_packet.h>

#include <SerialStream.h>

#define ARDUINO_COMMAND_CODE 7
#define DEFAULT_VELOCITY_VARIABLE "V"
#define DEFAULT_OMEGA_VARIABLE "W"

class zmq_real_world_serial_communicator : public agent_namespace::world_communicator_abstract,
public zmq_agent_to_simulator_communicator<agent_sim_packet_receiver,control_command_packet>
{
public:
zmq_real_world_serial_communicator(std::string agent_name,index_map& input_map);
const world_sim_packet& receive_agents_status();
void send_control_command( control_command_packet& packet, const target_abstract& );
inline const world_sim_packet& get_last_received()
{
  return packet_received;

}
~zmq_real_world_serial_communicator();

private:
world_sim_packet packet_received;
LibSerial::SerialStream serial_port;
index_map map_inputs_name_to_id;

};


#endif //ZMQ_REAL_WORLD_SERIAL_COMMUNICATOR_H
