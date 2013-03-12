#include "zmq_world_communicator.h"


zmq_world_communicator::zmq_world_communicator(std::string agent_name)
{
	init(agent_name);
}


const world_sim_packet& zmq_world_communicator::receive_agents_status()
{
  try{
    agent_sim_packet_receiver tmp=receive().front(); //TODO(Mirko): zerocopy here
    packet_received.state_agents.internal_map.clear();
    packet_received.bonus_variables=tmp.bonus_variables;
    packet_received.time=tmp.time;
    
    for (auto agent=tmp.state_agents.internal_map.begin();agent!=tmp.state_agents.internal_map.end();++agent){
    packet_received.state_agents.internal_map[agent->first]=*(agent->second);
      
    }
return  packet_received;
  }
   catch (zmq::error_t ex)
   {
     if (zmq_errno()==EINTR)
	  throw "programma terminato";
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
