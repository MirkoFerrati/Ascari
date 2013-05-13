#ifndef AGENT_ROUTER_PLUGIN_H
#define AGENT_ROUTER_PLUGIN_H
#include "../abstract_plugin.h"
#include "agent_router.h"


class agent_router_plugin: public abstract_plugin
{
public:
    agent_router_plugin(){
      
    };
    abstract_agent_plugin* getAgentPlugin(){return agent;};


private:
   agent_router* agent;
   
};



#endif //AGENT_ROUTER_PLUGIN_H