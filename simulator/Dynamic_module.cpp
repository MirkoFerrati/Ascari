#include "Dynamic_module.h"

void Dynamic_module::evolve(agents_name_to_states& states, const std::vector< control_command_packet >& commands)
{
    for (unsigned int i=0;i<commands.size();i++)
    {
        states.internal_map[commands[i].identifier].state=dynamics[agents_name_to_dynamics[commands[i].identifier]].getNextState(states.internal_map[commands[i].identifier].state,commands[i].command);
		std::cout<<commands[i].identifier<<" "<<states.internal_map[commands[i].identifier].state[1]<<std::endl;
	}

}
