#ifndef AGENT_ROUTER_LOGGING_H
#define AGENT_ROUTER_LOGGING_H
#include <types.h>
#include <logog.hpp>
struct startInfo{
    
};

struct position{
    
};

struct speed{
    
};

class agent_router_logging
{
public:
    agent_router_logging(simulation_time& time, std::string name);
    static inline void logStartInfo(simulation_time time, std::string identifier, int source, int target, double startx, double starty)
    {
        INFO("StartInfo: %g %s source %d target %d startX %g startY %g",time,identifier.c_str(),source,target,startx,starty);
    }
    void logBestLength(int best_distance);
    void logPosition(double x, double y, double theta);
    void logSpeed(double speed, double omega);
    void logFinalTarget();
    void logNewTarget(int id, double x, double y);
    void logFutureCollision(std::string other_agent,int node_id, int steps);
    startInfo parseStartInfo();
    position parsePosition();
    speed parseSpeed();
private:
    simulation_time& time;
    std::string name;
    
};

#endif // AGENT_ROUTER_LOGGING_H
