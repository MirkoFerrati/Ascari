#ifndef ZMQ_FULL_COMMUNICATOR_HPP
#define ZMQ_FULL_COMMUNICATOR_HPP
#include <zmq.h>
#include "zmq_communicator.hpp"

/**We define in the same file the couples of communication primitives, so we are sure that sockets pair in the right way
 * */

template <class receive_type, class send_type>
class zmq_agent_to_simulator_communicator : 
public zmq_communicator<receive_type,send_type,ZMQ_PUSH,ZMQ_SUB,ASK_SYNC>
{
public:
	void init(std::string agent_name)
	{
		this->init_full(agent_name,true,"tcp://127.0.0.1:5555","tcp://127.0.0.1:5556","tcp://127.0.0.1:5557",1,false,false);
	}
};

template <class receive_type,class send_type>
class zmq_simulator_to_agent_communicator: 
public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,WAIT_SYNC>
{
public:
	void init(unsigned int expected_senders=1)
	{
		this->init_full("simulator",false,"tcp://127.0.0.1:5556","tcp://127.0.0.1:5555","tcp://127.0.0.1:5557",expected_senders,true,true);
	}
};


template <class receive_type,class send_type>
class zmq_simulator_to_viewer_communicator: 
public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,NO_SYNC>
{
public:
  void init(unsigned int expected_senders=1)
  {
    this->init_full("simulator",false,"tcp://127.0.0.1:5558","tcp://127.0.0.1:5559","",expected_senders,true,true);
  }
};

template <class receive_type>
class zmq_sniffer_simulator_communicator : 
public zmq_receive_communicator<receive_type,ZMQ_SUB>
{
public:
	void init(std::string owner_name)
	{
		this->init_full(owner_name,true,"tcp://127.0.0.1:5559",1,false);
	}
};

#endif //ZMQ_FULL_COMMUNICATOR_HPP