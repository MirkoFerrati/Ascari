#ifndef DUMMY_AGENT_HPP
#define DUMMY_AGENT_HPP

#include <stdio.h>
#include <mutex>
#include "agent_to_dummy_communicator.hpp"
#include "../agent/agent.h"
#include <string>

struct dummy_agent
{
  dummy_agent(std::string agent_name,const std::pair<const std::string, std::unique_ptr<Parsed_Behavior>>& behavior,int behavior_id, const Parsed_World & world,std::shared_ptr< world_communicator_abstract > communicator)
  :dummy(agent_name,behavior.second,world),behavior_identifier(behavior_id),identifier(agent_name)
  {
    //char temp[5];
    //snprintf(temp,5,"%d",behavior_id);
    identifier.append(std::to_string(behavior_id));
    
    dummy.setCommunicator(communicator);
    dummy.setControlCommandIdentifier(identifier);
    
    dummy_state tmp;
    for (auto dstate = dummy.getDiscreteStates().begin(); dstate != dummy.getDiscreteStates().end(); ++dstate) {
	    tmp.automatonState=*dstate;
            states.push_front(tmp);
    }
    
    
  }
public:
  agent dummy;
  int behavior_identifier;
  std::forward_list<dummy_state> states;
  std::string identifier;
  std::mutex comm_mutex;
   inline std::forward_list<dummy_state> & getDummyStates() {
        return states;
    }
};


#endif //DUMMY_AGENT_HPP