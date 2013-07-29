#ifndef AGENT_ROUTER_SIMULATOR_PLUGIN_H
#define AGENT_ROUTER_SIMULATOR_PLUGIN_H

#ifdef ISSIMULATOR

#include "udp_agent_router.hpp"
#include "../abstract_simulator_plugin.h"
#include <types/graph_informations.h>
#include "../simulator/simulator.h"


class agent_router_simulator_plugin : public abstract_simulator_plugin
{

public:
    
    virtual ~agent_router_simulator_plugin();
    virtual void stop();
    virtual bool initialize ( const Parsed_World& w );
    virtual void run_plugin();
    agent_router_simulator_plugin( simulator* s );
    
private:
  	Udp_agent_router<graph_packet>* graph_router;
};
#endif
#endif // AGENT_ROUTER_SIMULATOR_PLUGIN_H
