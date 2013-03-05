#ifndef AGENT_COMMUNICATOR_ABSTRACT_H
#define AGENT_COMMUNICATOR_ABSTRACT_H

#include "typedefs.h"
#include <vector>
#include <string>


namespace simulator_namespace
{
class agent_communicator_abstract
{
public:
	virtual std::vector<control_command_packet> receive_control_commands()=0;
	virtual void send_broadcast(const world_sim_packet& infos)=0;
	virtual void send_target(const world_sim_packet& infos, const target_abstract& target)=0;
	virtual ~agent_communicator_abstract(){};
};
}
#endif // AGENT_COMMUNICATOR_ABSTRACT_H
