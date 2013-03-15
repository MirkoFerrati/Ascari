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
    };
    
    
    
    ~agent_to_dummy_communicator (){
      buffer_control.clear();
      
    };
    
     const world_sim_packet& get_last_received(){
       throw "PERCHE? MI HAI CHIAMATO?";
     }
    
    const world_sim_packet& receive_agents_status(){
    
      if (!buf_agents.empty()){
	  return buf_agents.front();
	 }
      else
      {
	ERR("MONITOR: ESEGUITA RECEIVE PRIMA DI UNA SEND",NULL);
	throw "MONITOR: ESEGUITA RECEIVE PRIMA DI UNA SEND";
      }
	
      
    };
    
    void removeFront(){
      buf_agents.pop_front();
    }
    
    
    void send ( agent_sim_packet& infos) {
      
      world_sim_packet tmp;
      tmp.bonus_variables=infos.bonus_variables;
      tmp.time=infos.time;
      for (auto agent=infos.state_agents.internal_map.begin();agent!=infos.state_agents.internal_map.end();agent++){
      tmp.state_agents.internal_map[agent->first]=*(agent->second);
      }
      
      buf_agents.push_back(tmp);
      
      
    };
    
    void send_control_command (control_command_packet& control_command, const target_abstract& /*target*/)
    {
      
      buffer_control[control_command.identifier]=control_command;
      
      
    };
    
    control_command_packet receive_control_command (std::string const & source) {
      
      
      return buffer_control[source];
      
        
    };
    
public:

  std::map<std::string, control_command_packet > buffer_control;
  std::list<world_sim_packet> buf_agents;
  
  
};










#endif //AGENT_TO_DUMMY_COMMUNICATOR
