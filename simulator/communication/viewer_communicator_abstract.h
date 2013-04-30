#ifndef VIEWER_COMMUNICATOR_ABSTRACT_H
#define VIEWER_COMMUNICATOR_ABSTRACT_H


#include <vector>
#include <string>
#include <types/world_sim_packet.h>


namespace simulator_namespace
{
class viewer_communicator_abstract
{
public:
	virtual void send_broadcast(const world_sim_packet& infos)=0;
	virtual void send_target(const world_sim_packet& infos, const target_abstract& target)=0;
	virtual ~viewer_communicator_abstract(){};
};
}
#endif // VIEWER_COMMUNICATOR_ABSTRACT_H
