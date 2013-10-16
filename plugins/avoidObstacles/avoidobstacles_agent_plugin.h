#ifndef AVOIDOBSTACLES_AGENT_PLUGIN_H
#define AVOIDOBSTACLES_AGENT_PLUGIN_H
#include "../abstract_agent_plugin.h"
#include <agent.h>

class avoidObstacles_agent_plugin: public abstract_agent_plugin
{
public:
    avoidObstacles_agent_plugin(agent* a);
    virtual void run_plugin();
    virtual bool initialize();
    const world_sim_packet* agents;
    double &x,&y;
    double &theta;
    double &speed;
    double &omega;
private:
    int x_index;
    int y_index;
    int theta_index;

    bool collision(double x, double y, double theta,double xa,double ya);
    
};

#endif // AVOIDOBSTACLES_AGENT_PLUGIN_H
