#ifndef ZMQ_FULL_COMMUNICATOR_HPP
#define ZMQ_FULL_COMMUNICATOR_HPP
#include <zmq.h>
#include "zmq_communicator.hpp"

/**We define in the same file the couples of communication primitives, so we are sure that sockets pair in the right way
 * */
//#define AGENT_TO_SIMULATOR "tcp://127.0.0.1:5555"
//#define SIMULATOR_TO_AGENT "tcp://127.0.0.1:5556"
//#define AGENT_SYNC_SIMULATOR "tcp://127.0.0.1:5557"
//#define SIMULATOR_VIEWER "tcp://127.0.0.1:5559"

#define AGENT_TO_SIMULATOR CONFIG.getValue("AGENT_TO_SIMULATOR")
#define SIMULATOR_TO_AGENT CONFIG.getValue("SIMULATOR_TO_AGENT")
#define AGENT_SYNC_SIMULATOR CONFIG.getValue("AGENT_SYNC_SIMULATOR")
#define SIMULATOR_VIEWER CONFIG.getValue("SIMULATOR_VIEWER")

template <class receive_type, class send_type>
class zmq_agent_to_simulator_communicator : 
public zmq_communicator<receive_type,send_type,ZMQ_PUSH,ZMQ_SUB,ASK_SYNC>
{
public:
	void init(std::string agent_name)
	{
		this->init_full(agent_name,true,AGENT_TO_SIMULATOR,SIMULATOR_TO_AGENT,AGENT_SYNC_SIMULATOR,1,false,false);
	}
};

template <class receive_type,class send_type>
class zmq_simulator_to_agent_communicator: 
public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,WAIT_SYNC>
{
public:
	void init(unsigned int expected_senders=1)
	{
		this->init_full("simulator",false,SIMULATOR_TO_AGENT,AGENT_TO_SIMULATOR,AGENT_SYNC_SIMULATOR,expected_senders,true,true);
	}
};


template <class receive_type,class send_type>
class zmq_simulator_to_viewer_communicator: 
public zmq_send_communicator<send_type,ZMQ_PUB>
{
public:
  void init(unsigned int expected_senders=1)
  {
    this->init_full("simulator",SIMULATOR_VIEWER,true);
  }
};

template <class receive_type>
class zmq_sniffer_simulator_communicator : 
public zmq_receive_communicator<receive_type,ZMQ_SUB>
{
public:
	void init(std::string owner_name)
	{
		this->init_full(owner_name,true,SIMULATOR_VIEWER,1,false);
	}
};

#endif //ZMQ_FULL_COMMUNICATOR_HPP