#ifndef DYNAMIC_H
#define DYNAMIC_H
#include "typedefs.h"

class dynamic
{
public:
	dynamic();
	agentState getNextState(const agentState& oldState,const controlCommand& control ) const;
};

#endif // DYNAMIC_H
