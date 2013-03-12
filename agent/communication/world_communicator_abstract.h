#ifndef WORLD_COMMUNICATOR_H
#define WORLD_COMMUNICATOR_H

#include "typedefs.h"

#include <vector>
namespace agent_namespace
{

class world_communicator_abstract
{
public:
	virtual const world_sim_packet& receive_agents_status()=0;
	virtual const world_sim_packet& get_last_received()=0;

	virtual void send_control_command(control_command_packet&, const target_abstract& target="")=0;
	virtual ~world_communicator_abstract(){};
	
};
}
#endif // WORLD_COMMUNICATOR_H