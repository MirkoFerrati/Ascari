#ifndef DUMMY_AGENT_HPP
#define DUMMY_AGENT_HPP

#include <stdio.h>
#include <mutex>
#include "agent_to_dummy_communicator.hpp"
#include "../agent/agent.h"

struct dummy_agent
{
  dummy_agent(std::string agent_name,const std::pair<const std::string, std::unique_ptr<Parsed_Behavior>>& behavior,int behavior_id, const Parsed_World & world, world_communicator_abstract * communicator)
  :dummy(agent_name,behavior.second,world),behavior_identifier(behavior_id),identifier(agent_name)
  {
    //char temp[5];
    //snprintf(temp,5,"%d",behavior_id);
    identifier.append(std::to_string(behavior_id));
    
    dummy.setCommunicator(communicator);
    dummy.setControlCommandIdentifier(identifier);
  }
public:
  agent dummy;
  int behavior_identifier;
  std::string identifier;
  std::mutex comm_mutex;
};


#endif //DUMMY_AGENT_HPP