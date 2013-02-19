#include "zmq_world_communicator.h"


zmq_world_communicator::zmq_world_communicator(std::string agent_name)
{
	init(agent_name);
}


world_sim_packet zmq_world_communicator::receive_agents_status()
{
  try{
return receive().front(); //TODO(Mirko): zerocopy here
  }
   catch (zmq::error_t ex)
   {
     if (zmq_errno()==EINTR)
	  throw "programma terminato";
     else 
       throw ex;
   }
  }


void zmq_world_communicator::send_control_command(control_command_packet& packet, const agent_namespace::target_abstract* /*target*/)
{
send(packet);
}


zmq_world_communicator::~zmq_world_communicator()
{

}
