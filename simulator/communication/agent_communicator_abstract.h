#ifndef AGENT_COMMUNICATOR_ABSTRACT_H
#define AGENT_COMMUNICATOR_ABSTRACT_H

#include "typedefs.h"
#include <vector>
#include <string>


struct target_abstract
{
	std::string identifier;
};

class agent_communicator_abstract
{
public:
	virtual void send_broadcast(const simulation_time&)=0;
	virtual std::vector<control_command_packet> receive_control_commands()=0;
	virtual void send_broadcast(const world_sim_packet& infos)=0;
	virtual void send_target(const world_sim_packet& infos, const target_abstract* target)=0;
	virtual ~agent_communicator_abstract(){};
};

#endif // AGENT_COMMUNICATOR_ABSTRACT_H
