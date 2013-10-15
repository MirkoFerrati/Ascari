#include "avoidobstacles_agent_plugin.h"

avoidObstacles_agent_plugin::avoidObstacles_agent_plugin(agent* a):agents(a->agents),x(a->state.at(a->map_statename_to_id.at("X"))),
y(a->state.at(a->map_statename_to_id.at("Y"))),theta(a->state.at(a->map_statename_to_id.at("THETA"))),
speed(a->inputs.command.at(a->map_inputs_name_to_id.at("V"))),omega(a->inputs.command.at(a->map_inputs_name_to_id.at("W")))
{

}


bool avoidObstacles_agent_plugin::initialize()
{
  
}

void avoidObstacles_agent_plugin::run_plugin()
{

}
