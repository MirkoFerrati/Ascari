#ifndef AGENT_H
#define AGENT_H


#include <vector>
#include <string>
#include <map>
#include <qgraphicsitem.h>
#include <types/agent_state_packet.h>


class Agent
{

  public:
    Agent();
    Agent(const std::string& name);
    ~Agent();


  public:
	  
	inline void translate(const agent_state_packet& agent)
	{
		std::map<int,double>::const_iterator temp;
		if ((temp=agent.state.find(x_index))!=agent.state.end())
			x=temp->second;
		if ((temp=agent.state.find(y_index))!=agent.state.end())
			y=temp->second;
		if ((temp=agent.state.find(angle_index))!=agent.state.end())
			angle=temp->second;
		if (this->maxX<x)
		this->maxX=x;
		if (this->maxY<y)
		this->maxY=y;
		
		if (this->minX>x)
		this->minX=x;
		if (this->minY>y)
		this->minY=y;
		
		//written by Alessandro Settimi
		if((temp=agent.state.find(charge_index))!=agent.state.end())
		  charge=temp->second;
		//written by Alessandro Settimi
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
    
    //written by Alessandro Settimi
    double charge;
    
    //written by Alessandro Settimi
    QGraphicsTextItem* text;
    QGraphicsPolygonItem *shape;
    bool created=false;
    
private:
    std::string agent_name;
	double maxX,maxY,minX,minY;
    int angle_index;
    int x_index;
    int y_index;
    int charge_index;
    void init();
	
};


#endif // AGENT_H
