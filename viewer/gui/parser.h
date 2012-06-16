#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include "AgentPath.h"
#include <string>
#include <map>
#include "wall.h"
#include "TargetList.h"
#include "set"
class Parser
{
    map<string, AgentPath > paths;
    vector<wallState> walls;
    map<string, vector<TargetList> > targets;
    int endtime;
    string imagePath;
    set<string> agents_name;
public:
    Parser(int argc,char *argv[]);
    int GetAgentNumber();
    set<string> getAgentsName()
    {
        return agents_name;
    }

    AgentPath getPaths(string agent_name)
    {
        return paths.at(agent_name);
    }

     vector<wallState> getWalls()
    {
        return walls;
    }
    string toString();
    string getImagePath(){
        return imagePath;
    }

    vector<TargetList> getTargets(string agent_name)
    {
        vector<TargetList> v;
        if (targets.count(agent_name))
            return targets.at(agent_name);
        return v;
    }

    int getTime();
    int getMaxX();
    int getMaxY();
    int getMinX();
    int getMinY();
};

#endif // PARSER_H
