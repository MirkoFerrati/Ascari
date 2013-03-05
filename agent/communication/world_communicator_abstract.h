#ifndef WORLD_COMMUNICATOR_H
#define WORLD_COMMUNICATOR_H

#include "typedefs.h"

#include <vector>
namespace agent_namespace
{

class world_communicator_abstract
{
public:
	virtual agent_sim_packet_receiver receive_agents_status()=0;

	virtual void send_control_command(control_command_packet&, const target_abstract& target)=0;
	virtual ~world_communicator_abstract(){};
};
}
#endif // WORLD_COMMUNICATOR_H