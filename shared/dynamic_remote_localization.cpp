#include "dynamic_remote_localization.h"

dynamic_remote_localization::dynamic_remote_localization(zmq_localization_communicator_receiver* receiver,std::string agent_name,std::map<std::string,agent_state> state):receiver(receiver),my_agent(agent_name)
{
  receiver->addAgent(agent_name,state);
}


agent_state dynamic_remote_localization::getNextState()
{
  return receiver->getState(my_agent);
}
