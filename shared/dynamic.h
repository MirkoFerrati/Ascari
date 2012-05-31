#ifndef DYNAMIC_H
#define DYNAMIC_H
#include "typedefs.h"

class dynamic
{
public:
	dynamic();
	agent_state getNextState(const agent_state& oldState, const control_command& control);
};

#endif // DYNAMIC_H
