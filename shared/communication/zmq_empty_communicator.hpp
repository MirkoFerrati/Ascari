//written by Alessandro Settimi

#include <zmq.h>
#include "zmq_communicator.hpp"


template <class receive_type, class send_type>
class agent_to_simulator_empty_communicator : public zmq_communicator<receive_type,send_type,ZMQ_PUSH,ZMQ_SUB,NO_SYNC>
{
public:
	void init(std::string agent_name)
	{
		this->init_full(agent_name,false,"tcp://127.0.0.1:5580","tcp://127.0.0.1:5581","tcp://127.0.0.1:5582",1,false,false);
	}
};

template <class receive_type,class send_type>
class simulator_to_agent_empty_communicator: public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,NO_SYNC>
{
public:
	void init(unsigned int expected_senders=1)
	{
		this->init_full("simulator",false,"tcp://127.0.0.1:5581","tcp://127.0.0.1:5580","tcp://127.0.0.1:5582",expected_senders,true,true);
	}
};
