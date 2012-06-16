#ifndef AGENT_H
#define AGENT_H




#include "AgentPath.h"
#include "TargetList.h"

class Agent
{

  public:
    Agent();
    Agent(const string& name,AgentPath p, vector< TargetList> t);
    ~Agent();


  public:
    void resetState();
//    void moveLeft(int);
//    void moveRight(int);
//    void moveTop(int);
//    void moveBot(int);
    void translate(int time);

    void setPath(AgentPath path);
    void setTargets(vector<TargetList> t);
    TargetList getTargets();
    TargetList getTargets(int time);
    double angle;
    double x;
    double y;
    TargetList targetlist;
  private:
    string agent_name;
    vector<TargetList> targets;
    AgentPath path;
};


#endif // AGENT_H
