#ifndef AGENT_H
#define AGENT_H


#include <vector>
#include <string>
#include <typedefs.h>


class Agent
{

  public:
    Agent();
    Agent(const std::string& name);
    ~Agent();


  public:
	  
	inline void translate(const agent_state_packet& agent)
	{
		x=agent.state.at(x_index);
		y=agent.state.at(y_index);
		angle=agent.state.at(angle_index);
		if (this->maxX<x)
		this->maxX=x;
		if (this->maxY<y)
		this->maxY=y;
		
		if (this->minX>x)
		this->minX=x;
		if (this->minY>y)
		this->minY=y;
		
	}
	  
    inline double getMaxX(){
		return maxX;
    }
    
    inline double getMinX(){
		return minX;
    }
    inline double getMaxY(){
		return maxY;
    }
    inline double getMinY(){
		return minY;
    }
    
    
    double y;
    double x;
    double angle;


private:
    std::string agent_name;
	double maxX,maxY,minX,minY;
    int angle_index;
    int x_index;
    int y_index;

	
};


#endif // AGENT_H
