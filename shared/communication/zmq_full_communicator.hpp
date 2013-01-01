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
		this->init_full(agent_name,"tcp://127.0.0.1:5555","tcp://127.0.0.1:5556","tcp://*:5557");
	}
};

template <class receive_type,class send_type>
class zmq_simulator_to_agent_communicator: 
public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,WAIT_SYNC>
{
public:
	void init(unsigned int expected_senders=1)
	{
		this->init_full("simulator","tcp://127.0.0.1:5556","tcp://127.0.0.1:5555","tcp://*:5557",expected_senders);
	}
};