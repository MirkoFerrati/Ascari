#ifndef FORMATION_CONTROL_COMMUNICATOR
#define FORMATION_CONTROL_COMMUNICATOR

#include <zmq.h>
#include "zmq_communicator.hpp"

#define AGENT_TO_SIMULATOR "tcp://127.0.0.1:7000"
#define SIMULATOR_TO_AGENT "tcp://127.0.0.1:7001"
#define AGENT_SYNC_SIMULATOR "tcp://127.0.0.1:7002"


template <class receive_type, class send_type>
class agent_to_simulator_formation_control_communicator : public zmq_communicator<receive_type,send_type,ZMQ_PUSH,ZMQ_SUB,ASK_SYNC>
{
public:
  void init(std::string agent_name)
  {
    this->init_full(agent_name, false, AGENT_TO_SIMULATOR, SIMULATOR_TO_AGENT, AGENT_SYNC_SIMULATOR, 1, false, false);
  }
};


template <class receive_type, class send_type>
class simulator_to_agent_formation_control_communicator: public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,WAIT_SYNC>
{
public:
  void init(unsigned int expected_senders = 1)
  {
    this->init_full("formation_control_simulator_plugin", false, SIMULATOR_TO_AGENT, AGENT_TO_SIMULATOR, AGENT_SYNC_SIMULATOR, expected_senders, true, true);
  }
};

#endif //FORMATION_CONTROL_COMMUNICATOR