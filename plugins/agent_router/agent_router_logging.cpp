#include "agent_router_logging.h"

agent_router_logging::agent_router_logging(simulation_time& time, std::string name):time(time),name(name)
{

}

void agent_router_logging::logBestLength(int best_distance)
{
    INFO("BestLength: %g %s %d",time, name.c_str(),best_distance);
}

void agent_router_logging::logSpeed(double speed, double omega)
{
    INFO("Speed: %g %s %g %g",time, name.c_str(),speed,omega);
}

void agent_router_logging::logPosition(double x, double y, double theta)
{
    INFO("Position: %g %s %g %g %g",time,name.c_str(),x,y,theta);
}

void agent_router_logging::logFutureCollision(std::string other_agent, int node_id, int steps)
{
    INFO("FutureCollision: %g %s %s %d %d",time, name.c_str(),other_agent.c_str(),node_id,steps);
}

void agent_router_logging::logFinalTarget()
{
    INFO("Target_reached: %g %s",time,name.c_str());
}

void agent_router_logging::logNewTarget(int id, double x,double y)
{
    INFO("New_target: %g %s %i %i %i",time, name.c_str(),id,x,y);    
}

