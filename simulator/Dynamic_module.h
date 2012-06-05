#ifndef DYNAMIC_MODULE_H
#define DYNAMIC_MODULE_H

#include "typedefs.h"
#include <vector>
#include <dynamic.h>

#include <string>

class Dynamic_module
{
public:
	Dynamic_module(agents_name_to_states& old_states, const std::vector< control_command_packet >& commands,
	std::map<std::string,std::string> expressions, std::vector<std::string> variables);
private:
	std::vector<dynamic> dynamics;
	index_map agents_name_to_dynamics;
public:
void evolve();

};

#endif // DYNAMIC_MODULE_H
