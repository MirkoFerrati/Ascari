#include "agent_router_logging.h"

#define position_string "Position: %g %s %g %g %g"
#define speed_string "Speed: %g %s %g %g"
#define bestLength_string "BestLength: %g %s %d"
#define futureCollision_string "FutureCollision: %g %s %s %d %d"
#define finalTarget_string "Target_reached: %g %s"

agent_router_logging::agent_router_logging(simulation_time& time, std::string name):time(time),name(name)
{

}

void agent_router_logging::logBestLength(int best_distance)
{
    INFO(bestLength_string,time, name.c_str(),best_distance);
}
bestLength agent_router_logging::parseBestLength(std::string line_to_parse)
{
    char buffer[200];
    int temp;
    simulation_time time;
    if (line_to_parse.find("BestLength")==std::string::npos)
        throw "cannot parse file: BestLength tag not found";
    sscanf(line_to_parse.c_str(),"%s %lf %s %d",buffer,&time, buffer,&temp);
    return std::make_pair(std::string(buffer),temp);
}


void agent_router_logging::logSpeed(double speed, double omega)
{
    INFO(speed_string,time, name.c_str(),speed,omega);
}
speed agent_router_logging::parseSpeed(std::string line_to_parse)
{
    char buffer[200];
    speed temp;
    if (line_to_parse.find("Speed")==std::string::npos)
        throw "cannot parse file: Speed tag not found";
    sscanf(line_to_parse.c_str(),"%s %lf %s %lf %lf",buffer,&temp.time, buffer,&temp.speed,&temp.omega);
    temp.name=buffer;
    return temp;
}

void agent_router_logging::logPosition(double x, double y, double theta)
{
    INFO(position_string,time,name.c_str(),x,y,theta);
}
position agent_router_logging::parsePosition(std::string line_to_parse)
{
    char buffer[200];
    position temp;
    if (line_to_parse.find("Position")==std::string::npos)
        throw "cannot parse file: Position tag not found";
    sscanf(line_to_parse.c_str(),"%s %lf %s %lf %lf %lf",buffer,&(temp.time),buffer,&(temp.x),&(temp.y),&(temp.theta));
    temp.name=buffer;
    return temp;
}

void agent_router_logging::logFutureCollision(std::string other_agent, int node_id, int steps)
{
    INFO(futureCollision_string,time, name.c_str(),other_agent.c_str(),node_id,steps);
}
futureCollision agent_router_logging::parseFutureCollision(std::string line_to_parse)
{
    char buffer[200];
    char buffer1[200];
    futureCollision temp;
    if (line_to_parse.find("FutureCollision")==std::string::npos)
        throw "cannot parse file: FutureCollision tag not found";
    sscanf(line_to_parse.c_str(),"%s %lf %s %s %d %d",buffer,&temp.time,buffer,buffer1,&temp.node_id,&temp.steps);
    temp.name=buffer;
    temp.other_agent=buffer1;
    return temp;
}

collision agent_router_logging::parseCollision(std::string line_to_parse)
{
    char buffer[200];
    char buffer1[200];
    collision temp;
    if (line_to_parse.find("Collision:")==std::string::npos)
        throw "cannot parse file: Collision tag not found";
    sscanf(line_to_parse.c_str(),"%s %lf %s %s",buffer,&(temp.time),buffer,buffer1);
    temp.name=buffer;
    temp.other_name=buffer1;
    return temp;
}


void agent_router_logging::logFinalTarget()
{
    INFO(finalTarget_string,time,name.c_str());
}
targetReached agent_router_logging::parseFinalTarget(std::string line_to_parse)
{
    char buffer[200];
    simulation_time time;
    if (line_to_parse.find("Target_reached")==std::string::npos)
        throw "cannot parse file: Target_reached tag not found";
    sscanf(line_to_parse.c_str(),"%s %lf %s",buffer,&time,buffer);
    return std::make_pair(std::string(buffer),time);
}


void agent_router_logging::logNewTarget(int id, double x,double y)
{
    INFO("New_target: %g %s %i %i %i",time, name.c_str(),id,x,y);    
}


startInfo agent_router_logging::parseStartInfo(std::string line_to_parse)
{   
    char buffer[200];
    char buffer1[200];
    startInfo temp;
    if (line_to_parse.find("StartInfo")==std::string::npos)
        throw "cannot parse file: StartInfo tag not found";
    sscanf(line_to_parse.c_str(),"%s %lf %s %s %d %s %d %s %lf %s %lf",buffer1,&temp.time,buffer,buffer1,&temp.source,buffer1,&temp.target,buffer1,&temp.startx,buffer1,&temp.starty);
    temp.name=buffer;
    return temp;
}
