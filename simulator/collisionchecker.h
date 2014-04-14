#ifndef COLLISIONCHECKER_H
#define COLLISIONCHECKER_H

#include "collisioncheckerabstract.h"
#include <types/agents_name_to_states.h>
#include <types/agent_state_packet.h>
#include <vector>

struct Point
{
    int x;
    int y;
    Point(double x,double y):x(x),y(y){
        
    }
    Point(){
        x=0;
        y=0;
    }
};
class CollisionChecker : public CollisionCheckerAbstract
{

public:
    virtual ~CollisionChecker();
    CollisionChecker(const agents_name_to_states& states,const index_map & states_to_index);
	bool checkCollisions(simulation_time& time);
private:
    const std::map<std::string,agent_state_packet>& states;
    std::vector<Point> getPoints(Point center,double theta, double h,double l);
    Point localToGlobal(Point center, double theta, Point p);
    bool isInside(Point center, double theta, double h, double l, Point p);
    
    
	std::vector<int> indexes_to_check;
};

#endif // COLLISIONCHECKER_H
