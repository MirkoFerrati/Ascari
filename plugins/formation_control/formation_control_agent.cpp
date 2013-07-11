
#include "formation_control_agent.h"
#include "../agent/agent.h"
#include "formation_control_plugin.h"


formation_control_agent::formation_control_agent( agent* a, Parsed_World* parse )
:my_id(a->identifier)
{
  x0 = parse->agents.front().initial_states.at("X");
  y0 = parse->agents.front().initial_states.at("Y");
}


void formation_control_agent::run_plugin()
{
  v=5;
  w=0;
  std::cout << x.value() << " " << y.value() << std::endl;
}