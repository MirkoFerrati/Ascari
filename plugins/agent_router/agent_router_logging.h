#ifndef AGENT_ROUTER_LOGGING_H
#define AGENT_ROUTER_LOGGING_H
#include <types.h>
#include <logog.hpp>

#define position_string "%s %g %s %g %g %g"
#define speed_string "%s %g %s %g %g"
#define bestLength_string "%s %g %s %d"
#define futureCollision_string "%s %g %s %s %d %d"
#define finalTarget_string "%s %g %s"
#define position_tag "P:"
#define speed_tag "S:"
#define bestLength_tag "B:"
#define futureCollision_tag "F:"
#define finalTarget_tag "T:"
#define startInfo_tag "I:"

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
        INFO("%s %g %s s %d t %d sX %g sY %g",startInfo_tag,time,identifier.c_str(),source,target,startx,starty);
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
