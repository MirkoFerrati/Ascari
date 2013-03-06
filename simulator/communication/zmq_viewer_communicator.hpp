#ifndef ZMQ_VIEWER_COMMUNICATOR_H
#define ZMQ_VIEWER_COMMUNICATOR_H
#include <communication/zmq_full_communicator.hpp>
#include <viewer_communicator_abstract.h>
#include <zmq.hpp>

class zmq_viewer_communicator :
public simulator_namespace::viewer_communicator_abstract,zmq_simulator_to_viewer_communicator<world_sim_packet,world_sim_packet>
{
public:
zmq_viewer_communicator()
{
	init(1);
}

void send_broadcast(const world_sim_packet& infos)
{
	send(infos);
}


void send_target(const world_sim_packet& infos, const target_abstract& target)
{
    send(infos,target);
}


~zmq_viewer_communicator()
{
}

};






#endif //ZMQ_VIEWER_COMMUNICATOR_H