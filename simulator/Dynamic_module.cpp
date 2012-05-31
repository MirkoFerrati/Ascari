#include "Dynamic_module.h"

void Dynamic_module::evolve(agents_name_to_states& old_states, const std::vector< control_command_packet >& commands)
{
for (unsigned int i=0;i<commands.size();i++)
{
	old_states.internal_map[commands[i].identifier].state=dynamics[agents_name_to_dynamics[commands[i].identifier]].getNextState(old_states.internal_map[commands[i].identifier].state,commands[i].command);
}

}