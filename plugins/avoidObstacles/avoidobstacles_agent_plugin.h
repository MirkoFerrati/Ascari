#ifndef AVOIDOBSTACLES_AGENT_PLUGIN_H
#define AVOIDOBSTACLES_AGENT_PLUGIN_H
#include "../abstract_agent_plugin.h"
#include <agent.h>

class avoidObstacles_agent_plugin: public abstract_agent_plugin
{
    avoidObstacles_agent_plugin(agent* a);
    virtual void run_plugin();
    virtual bool initialize();
};

#endif // AVOIDOBSTACLES_AGENT_PLUGIN_H
