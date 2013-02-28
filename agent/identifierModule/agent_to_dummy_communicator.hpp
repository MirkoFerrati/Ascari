#ifndef AGENT_TO_DUMMY_COMMUNICATOR
#define AGENT_TO_DUMMY_COMMUNICATOR

#include "agent.h"
#include "dummy_agent.hpp"
#include "../agent/communication/world_communicator_abstract.h"
#include "typedefs.h"


class agent_to_dummy_communicator : public agent_namespace::world_communicator_abstract
{
public:
    
    agent_to_dummy_communicator ()
    {
     buf_agents=0;
    };
    
    ~agent_to_dummy_communicator (){
      buffer_control.clear();

    };
    
    
    
    world_sim_packet receive_agents_status(){
    
      if (buf_agents!=0)
	  return *buf_agents;
      else
      {
	ERR("MONITOR: ESEGUITA RECEIVE PRIMA DI UNA SEND",NULL);
	throw "MONITOR: ESEGUITA RECEIVE PRIMA DI UNA SEND";
      }
	
      
    };
    
    
    void send ( world_sim_packet* infos) {
      
      buf_agents = infos;
    };
    
    void send_control_command (control_command_packet& control_command, const agent_namespace::target_abstract* target)
    {
      
      //buffer_control.insert(make_pair(control_command.identifier,control_command));
      buffer_control[control_command.identifier]=control_command;
      
      
    };
    
    control_command_packet receive_control_command (std::string const & source) {
      
      
      return buffer_control[source];
      
        
    };
    
public:

  world_sim_packet *buf_agents;
  std::map<std::string, control_command_packet > buffer_control;
  
};










#endif //AGENT_TO_DUMMY_COMMUNICATOR
