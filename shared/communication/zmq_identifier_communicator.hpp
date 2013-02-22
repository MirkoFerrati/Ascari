
#include <zmq.h>
#include "zmq_communicator.hpp"


template <class send_type>
class identifier_to_viewer_communicator : public zmq_communicator<send_type,send_type,ZMQ_PUB,ZMQ_SUB,NO_SYNC>
{
public:
	void init(std::string agent_name)
	{
		this->init_full(agent_name,"tcp://127.0.0.1:5580","tcp://127.0.0.1:5581","",1,false,false);
	}
private:
	std::vector<send_type> receive()=delete;
};

template <class receive_type>
class viewer_from_identifier_sniffer: public zmq_receive_communicator<receive_type,ZMQ_SUB>
{
public:
	void init(unsigned int expected_senders=1)
	{
		this->init_full("viewer_identifier_sniffer","tcp://127.0.0.1:5581",expected_senders,true);
	}
};
