#ifndef AGENT_ROUTER_LOGGING_H
#define AGENT_ROUTER_LOGGING_H
#include <types.h>
#include <logog.hpp>
struct startInfo{
    std::string name;
    simulation_time time;
    int source;
    int target;
    double startx; 
    double starty;
};

typedef  std::pair<std::string, int> bestLength;
typedef  std::pair<std::string, simulation_time> targetReached;

struct futureCollision{
    std::string name;
    std::string other_agent;
    int node_id;
    int steps;
    simulation_time time;
};


struct position{
    std::string name;
    double x;
    double y;
    double theta;
    simulation_time time;
};

struct speed{
    std::string name;
    double speed;
    double omega;
    simulation_time time;
};

struct collision{
    std::string name;
    std::string other_name;
    simulation_time time;
};

class agent_router_logging
{
public:
    agent_router_logging(simulation_time& time, std::string name);
    static inline void logStartInfo(simulation_time time, std::string identifier, int source, int target, double startx, double starty)
    {
        INFO("StartInfo: %g %s source %d target %d startX %g startY %g",time,identifier.c_str(),source,target,startx,starty);
    }
    
    static collision parseCollision(std::string line_to_parse);
    
    startInfo parseStartInfo(std::string line_to_parse);
    
    void logBestLength(int best_distance);
    bestLength parseBestLength(std::string line_to_parse);
    
    void logPosition(double x, double y, double theta);
    position parsePosition(std::string line_to_parse);
    
    void logSpeed(double speed, double omega);
    speed parseSpeed(std::string line_to_parse);
    
    void logFinalTarget();
    targetReached parseFinalTarget(std::string line_to_parse);
    
    void logNewTarget(int id, double x, double y);
    
    void logFutureCollision(std::string other_agent,int node_id, int steps);
    futureCollision parseFutureCollision(std::string line_to_parse);
    
private:
    simulation_time& time;
    std::string name;
    
};

#endif // AGENT_ROUTER_LOGGING_H
