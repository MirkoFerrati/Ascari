#ifndef DYNAMIC_MODULE_H
#define DYNAMIC_MODULE_H

#include "typedefs.h"
#include <vector>
#include <dynamic.h>

class Dynamic_module
{
public:
private:
	std::vector<dynamic> dynamics;
	index_map agents_name_to_dynamics;
public:
void evolve(agents_name_to_states& old_states, const std::vector< control_command_packet >& commands);

};

#endif // DYNAMIC_MODULE_H
