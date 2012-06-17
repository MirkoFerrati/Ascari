
#include "Agent.h"
#include <iostream>


Agent::Agent()
{
x_index=0;
y_index=1;
angle_index=2;
minX=minY=maxX=maxY=0;

}

Agent::Agent(const std::string& name):agent_name(name)
{

}

Agent::~Agent()
{

}
