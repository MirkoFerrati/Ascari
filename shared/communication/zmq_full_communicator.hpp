#include <zmq.hpp>
#include "zmq_communicator.hpp"



template <class receive_type, class send_type>
class zmq_agent_to_simulator_communicator : public zmq_communicator<receive_type,1,send_type,ZMQ_PUSH,ZMQ_SUB>
{
	
};

template <class receive_type,int expected_senders,class send_type>
class zmq_simulator_to_agent_communicator: public zmq_communicator<receive_type,expected_senders,send_type,ZMQ_PUB,ZMQ_PULL>
{
	
};