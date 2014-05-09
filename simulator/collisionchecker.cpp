#include "collisionchecker.h"
#include <logog.hpp>
#include <mutex>
#include <boost/concept_check.hpp>

#define COLLISION_RANGE 5

extern std::map<std::string,bool> collisions;
extern std::map<std::string,bool> should_check;
extern std::mutex collisions_mutex;
extern std::mutex should_check_mutex;

using namespace std;

bool CollisionChecker::isInside(Point center, double theta, double h, double l, Point p)
{
    auto ct=cos(theta);
    auto st=sin(theta);
    auto deltax=p.x-center.x;
    auto deltay=p.y-center.y;
    return (abs(ct*deltax+st*deltay)<l/2.0) && (abs(-st*deltax+ct*deltay)<h/2.0);
}


Point CollisionChecker::localToGlobal(Point center, double theta, Point p)
{
    Point temp;
    temp.x=center.x+p.x*cos(theta)-p.y*sin(theta);
    temp.y=center.y+p.x*sin(theta)+p.y*cos(theta);
    return temp;
}

std::vector<Point> CollisionChecker::getPoints(Point center,double theta, double h,double l)
{
    std::vector<Point> temp;
    temp.push_back(localToGlobal(center,theta,Point(-l/2.0,-h/2.0)));
    temp.push_back(localToGlobal(center,theta,Point(l/2.0,h/2.0)));
    temp.push_back(localToGlobal(center,theta,Point(-l/2.0,+h/2.0)));    
    temp.push_back(localToGlobal(center,theta,Point(+l/2.0,-h/2.0)));
    return temp;
}

bool CollisionChecker::checkCollisions(simulation_time& time)
{
    
    /*
     * Copy old collision status (to avoid printing the same warning again and again)
     * reset new collision status
     * For all agents i
     *  For all agents j
     *   if (agents are near) collision=true;
     *   if (collision) verify real collision condition
     *   if (collision & not collision before) warn about collision       
     *   update collision status
     */
    
    bool ret=false;
    collisions_mutex.lock();
    auto collisions_old=collisions;
    for (auto t=collisions.begin();t!=collisions.end();++t)
        t->second=false;
    collisions_mutex.unlock();
    for (std::map<std::string,agent_state_packet>::const_iterator it=states.begin();it!=states.end();++it) {
        if (!should_check[it->first]) continue;
        for (std::map<std::string,agent_state_packet>::const_iterator itt=states.begin();itt!=states.end();++itt) {
            if (!should_check[itt->first]) continue;
            if (it.operator!=(itt))
            {
                bool collision=true;
                for (unsigned int i=0;i<indexes_to_check.size();i++)
                {
                    if (it->second.state.count(i) && itt->second.state.count(i))
                        collision=collision &&  (abs(it->second.state.at(i)-itt->second.state.at(i))<COLLISION_RANGE);
                }
                if (collision)
                {
                    bool pointInside=false;
                    auto points=getPoints(Point(it->second.state.at(0),it->second.state.at(1)),it->second.state.at(2),1.5,3.0);
                    for (Point& p : points)
                    {
                        if (isInside(Point(itt->second.state.at(0),itt->second.state.at(1)),itt->second.state.at(2),1.5,3.0,p))
                        {
                            pointInside=true;
                            break;
                        }
                    }
                    if (!pointInside)
                        collision=false;
                }
                collisions_mutex.lock();
                if (collision && !collisions_old[it->first] && !collisions_old[itt->first])
                {
                    WARN("Collision: %lf %s %s",time,it->first.c_str(),itt->first.c_str());
                    collisions[it->first]=true;
                    collisions[itt->first]=true;
                    //usleep(1000*500);
                    ret=true;
                }
                collisions_mutex.unlock();
            }
        }
    }
    return ret;
}


CollisionChecker::~CollisionChecker()
{

}

CollisionChecker::CollisionChecker(const agents_name_to_states& states, const index_map& states_to_index)
        :CollisionCheckerAbstract(states,states_to_index),states(states.internal_map)
{
    if (!(states_to_index.count("X") && states_to_index.count("Y")))
	{	
		ERR("impossibile effettuare il controllo sugli stati x e y perche' nessun agente possiede quegli stati",NULL);
		throw "impossibile effettuare il controllo sugli stati x e y perche' nessun agente possiede quegli stati";
	}
	if (states_to_index.count("X")) //TODO(Mirko): IssueID #50
	   indexes_to_check.push_back(states_to_index.at("X"));
	if (states_to_index.count("Y"))
	   indexes_to_check.push_back(states_to_index.at("Y"));
        for (auto agent:states.internal_map)
            should_check[agent.first]=true;
}
