#include "avoidobstacles_agent_plugin.h"

avoidObstacles_agent_plugin::avoidObstacles_agent_plugin(agent* a):a(a)
{

}


bool avoidObstacles_agent_plugin::collision(double x, double y, double theta, double xa, double ya )
{
    double distance=sqrt((x-xa)*(x-xa)+(y-ya)*(y-ya));
    double angle=atan2(ya-y,xa-x)-theta;
    if (distance<40 && abs(angle)<1) return true;
    return false;
}


bool avoidObstacles_agent_plugin::initialize()
{
    //Maybe load some parameters from file, no idea for now
    
    x=&(a->state.at(a->map_statename_to_id.at("X")));
    y=&(a->state.at(a->map_statename_to_id.at("Y")));
    theta=&(a->state.at(a->map_statename_to_id.at("THETA")));
    speed=&(a->inputs.command.at(a->map_inputs_name_to_id.at("V")));
    omega=&(a->inputs.command.at(a->map_inputs_name_to_id.at("W")));
    x_index=(a->map_statename_to_id.at("X"));
    y_index=(a->map_statename_to_id.at("Y"));
    theta_index=(a->map_statename_to_id.at("THETA"));
    return true;
}

void avoidObstacles_agent_plugin::run_plugin()
{
    *speed=0.3;
    *omega=0.3;
    agents=(a->agents);
for(auto agent:agents->state_agents.internal_map)
    {
        if (collision(*x,*y,*theta,agent.second.state.at(x_index),agent.second.state.at(y_index)))
        {
            *speed=0;
            *omega=0;
            break;
        }
    }
}
