#ifndef ENCODER_ABSTRACT_H
#define ENCODER_ABSTRACT_H

#include <map>
#include "typedefs.h"

class encoderAbstract
{
public:
	virtual void computeSubEvents(const std::map<std::string, agent_state_packet>& state_other_agents)=0;
	virtual ~encoderAbstract(){};
};

#endif // ENCODER_H