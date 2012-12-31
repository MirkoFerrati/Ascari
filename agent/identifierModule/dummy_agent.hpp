#ifndef DUMMY_AGENT_HPP
#define DUMMY_AGENT_HPP

#include <stdio.h>
#include <mutex>


struct dummy_agent
{
  dummy_agent(std::string agent_name,const std::pair<const std::string, std::unique_ptr<Parsed_Behavior>>& behavior,int behavior_id)
  :dummy(agent_name,behavior.second),behavior_identifier(behavior_id),identifier(agent_name)
  {
    char temp[5];
    snprintf(temp,5,"%d",behavior_id);
    identifier.append(temp);
  }
  agent dummy;
  int behavior_identifier;
  std::string identifier;
  std::mutex comm_mutex;;
};


#endif //DUMMY_AGENT_HPP