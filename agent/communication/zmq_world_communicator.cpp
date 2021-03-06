#include "zmq_world_communicator.h"


zmq_world_communicator::zmq_world_communicator(std::string agent_name)
{
	init(agent_name);
}


const world_sim_packet& zmq_world_communicator::receive_agents_status()
{
  try{
    world_sim_packet& tmp=receive().front();
    packet_received=tmp;
    return  packet_received;
  }
   catch (zmq::error_t ex)
   {
     if (zmq_errno()==EINTR)
     {
       ERR("programma terminato",NULL);
       return  packet_received;

     }
     else 
       throw ex;
   }
  }


void zmq_world_communicator::send_control_command(control_command_packet& packet, const target_abstract& /*target*/)
{
send(packet);
}


zmq_world_communicator::~zmq_world_communicator()
{

}
