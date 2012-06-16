
#include "Agent.h"
#include <iostream>


Agent::Agent()
{
//  image.load("Rectangle.png");
//  rect = image.rect();
  resetState();
}

Agent::Agent(const string& name,AgentPath p, vector<TargetList> t):agent_name(name),path(p),targets(t)
{
    resetState();
}

Agent::~Agent()
{

}

//void Agent::moveBot(int bot)
//{
//  if (rect.bottom() <= 400)
//    rect.moveTo(rect.left(), bot);
//}

//void Agent::moveTop(int top)
//{
//  if (rect.top() >= 2)
//    rect.moveTo(rect.left(), top);
//}

//void Agent::moveLeft(int left)
//{
//  if (rect.left() >= 2)
//    rect.moveTo(left, rect.top());
//}

//void Agent::moveRight(int right)
//{
//  if (rect.right() <= 298)
//    rect.moveTo(right, rect.top());
//}

void Agent::translate(int time)
{
    x=path.x[time];
    y=path.y[time];
    angle=path.a[time];
    targetlist=targets[time];
}

void Agent::resetState()
{
  if (path.x.size()==0)
  {
      x=0;
      y=0;
      angle=0;
  }
  else
      translate(0);
}

void Agent::setPath(AgentPath p)
{
    path=p;
}

void Agent::setTargets(vector<TargetList> t)
{
    targets=t;
}

TargetList Agent::getTargets()
{
    return targetlist;
}

TargetList Agent::getTargets(int time)
{
    return targets[time];
}
