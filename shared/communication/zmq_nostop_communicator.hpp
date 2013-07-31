#ifndef ZMQ_NOSTOP_COMMUNICATOR_HPP
#define ZMQ_NOSTOP_COMMUNICATOR_HPP
#include <zmq.h>
#include "zmq_communicator.hpp"

template <class receive_type, class send_type>
class agent_to_simulator_nostop_communicator : public zmq_communicator<receive_type,send_type,ZMQ_PUSH,ZMQ_SUB,ASK_SYNC>
{
public:
	void init(std::string agent_name)
	{
		this->init_full(agent_name,false,"tcp://127.0.0.1:5980","tcp://127.0.0.1:5981","tcp://127.0.0.1:5982",1,false,false);
	}
};

template <class receive_type,class send_type>
class simulator_to_agent_nostop_communicator: public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,WAIT_SYNC>
{
public:
	void init(unsigned int expected_senders=1)
	{
		this->init_full("nostop_simulator_plugin",false,"tcp://127.0.0.1:5981","tcp://127.0.0.1:5980","tcp://127.0.0.1:5982",expected_senders,true,true);
	}
};

#endif //ZMQ_NOSTOP_COMMUNICATOR_HPP