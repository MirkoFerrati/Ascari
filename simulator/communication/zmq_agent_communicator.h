#ifndef ZMQ_AGENT_COMMUNICATOR_H
#define ZMQ_AGENT_COMMUNICATOR_H
#include <agent_communicator_abstract.h>
#include <zmq.hpp>

class zmq_agent_communicator : public agent_communicator_abstract
{
public:
	
	zmq_agent_communicator(int num_agents);
	std::vector< control_command_packet > receive_control_commands();
	virtual void send_broadcast(const world_sim_packet& infos);
	void send_target(const world_sim_packet&  infos, const target_abstract* target);
    int subscribers_expected;
	
private:
	zmq::context_t context;
	zmq::socket_t publisher,syncservice;
	
	
};



#endif //ZMQ_AGENT_COMMUNICATOR_H