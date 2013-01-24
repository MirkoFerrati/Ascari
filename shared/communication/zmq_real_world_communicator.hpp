#include "zmq_communicator.hpp"



namespace real_world
{

template <class packet_type>
class agent_to_agent_communicator : public zmq_communicator<packet_type,packet_type,ZMQ_PUB,ZMQ_SUB>
{


};

}