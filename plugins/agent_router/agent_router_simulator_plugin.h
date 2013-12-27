#ifndef AGENT_ROUTER_SIMULATOR_PLUGIN_H
#define AGENT_ROUTER_SIMULATOR_PLUGIN_H

// #ifdef ISSIMULATOR

#include "udp_agent_router.hpp"
#include "agent_router_parser_plugin.h"
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

class simulator_router_plugin: public abstract_simulator_plugin_container
{
public:
    simulator_router_plugin()
    {
        simulator_plugin=0;
        parser_plugin=0;
    };
    
    
    std::string getType()
    {
        return AGENT_ROUTER_NAME;
    };
    
    abstract_simulator_plugin* getSimulatorPlugin()
    {
        return simulator_plugin;
    };
    bool createSimulatorPlugin ( simulator* s, Parsed_World* parse )
    {
        if ( simulator_plugin!=0 )
        {
            ERR ( "already created agent plugin, please check for double calls",NULL )
        }
        else
            simulator_plugin=new agent_router_simulator_plugin ( s);
        return simulator_plugin;
    };
    
    abstract_parser_plugin* getParserPlugin()
    {
        return parser_plugin;
    };
    
    bool createParserPlugin()
    {
        if ( parser_plugin!=0 )
        {
            ERR ( "already created agent plugin, please check for double calls",NULL )
        }
        else
            parser_plugin=new agent_router_parser_plugin ( getType());
        return parser_plugin;
    };
    
    bool isEnabled()
    {
        if (parser_plugin)
            return parser_plugin->isEnabled();
        return false;
    }
    
private:
    agent_router_simulator_plugin* simulator_plugin;
    agent_router_parser_plugin* parser_plugin;
    
};

#endif // AGENT_ROUTER_SIMULATOR_PLUGIN_H
