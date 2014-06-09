//written by Alessandro Settimi
#ifndef AGENT_TA_COMMUNICATOR
#define AGENT_TA_COMMUNICATOR
#include <zmq.h>
#include "zmq_communicator.hpp"
#define TA_AGENT_TO_SIMULATOR CONFIG.getValue("TA_AGENT_TO_SIMULATOR")
#define TA_SIMULATOR_TO_AGENT CONFIG.getValue("TA_SIMULATOR_TO_AGENT")
#define TA_AGENT_SYNC_SIMULATOR CONFIG.getValue("TA_AGENT_SYNC_SIMULATOR")


template <class receive_type, class send_type>
class agent_to_simulator_ta_communicator : public zmq_communicator<receive_type,send_type,ZMQ_PUSH,ZMQ_SUB,ASK_SYNC>
{
public:
	void init(std::string agent_name)
	{
		this->init_full(agent_name,false,TA_AGENT_TO_SIMULATOR,TA_SIMULATOR_TO_AGENT,TA_AGENT_SYNC_SIMULATOR,1,false,false);
	}
};

template <class receive_type,class send_type>
class simulator_to_agent_ta_communicator: public zmq_communicator<receive_type,send_type,ZMQ_PUB,ZMQ_PULL,WAIT_SYNC>
{
public:
	void init(unsigned int expected_senders=1)
	{
		this->init_full("task_assignment_simulator_plugin",false,TA_SIMULATOR_TO_AGENT,TA_AGENT_TO_SIMULATOR,TA_AGENT_SYNC_SIMULATOR,expected_senders,true,true);
	}
};

#endif //AGENT_TA_COMMUNICATOR