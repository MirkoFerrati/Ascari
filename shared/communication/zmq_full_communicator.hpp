#include <zmq.h>
#include "zmq_communicator.hpp"

/**We define in the same file the couples of communication primitives, so we are sure that sockets pair in the right way
 * */

template <class receive_type, class send_type>
class zmq_agent_to_simulator_communicator : public zmq_communicator<receive_type,send_type,ZMQ_PUSH,ZMQ_SUB,ASK_SYNC,1>
{
	
};

template <class receive_type,class send_type,int expected_senders=1>
class zmq_simulator_to_agent_communicator: public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,WAIT_SYNC,expected_senders>
{
	
};