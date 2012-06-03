#ifndef WORLD_COMMUNICATOR_H
#define WORLD_COMMUNICATOR_H

#include "typedefs.h"

#include <vector>

struct target_abstract
{
	std::string identifier;
};


class world_communicator_abstract
{
public:
	virtual simulation_time receive_time()=0;
	virtual agents_name_to_states receive_agents_status()=0;
	virtual void send_control_command(control_command_packet&, const target_abstract* target)=0;
};

#endif // WORLD_COMMUNICATOR_H