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
	virtual std::map<std::string,agent_state_packet> receive_agents_status()=0;
	virtual std::map<std::string,double> receive_bonus_variables()=0;
	virtual void send_control_command(control_command_packet&, const target_abstract* target)=0;
	virtual ~world_communicator_abstract(){};
};

#endif // WORLD_COMMUNICATOR_H