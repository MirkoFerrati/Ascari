#ifndef DYNAMIC_MODULE_ABSTRACT_H
#define DYNAMIC_MODULE_ABSTRACT_H

#include "types.h"

class dynamic_module_abstract
{
public:

	virtual agent_state getNextState()=0;
	virtual ~dynamic_module_abstract(){};
	
	
private:
};

#endif // DYNAMIC_MODULE_ABSTRACT_H
