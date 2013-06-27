#include "dynamic_remote_localization.h"

dynamic_remote_localization::dynamic_remote_localization()
{
  receiver->addAgent(agent_name,state);
}


agent_state dynamic_remote_localization::getNextState()
{
  return receiver->getState(my_agent);
}
