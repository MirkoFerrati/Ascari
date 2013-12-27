#ifndef AGENT_ROUTER_PLUGIN_H
#define AGENT_ROUTER_PLUGIN_H
#include "../abstract_agent_plugin.h"
#include "agent_router_parser_plugin.h"
#include "agent_router.h"

class agent_router_plugin_container: public abstract_agent_plugin_container
{
public:
    agent_router_plugin_container()
    {
        agent_plugin=0;
        parser_plugin=0;
    };


    std::string getType()
    {
        return AGENT_ROUTER_NAME;
    };

    abstract_agent_plugin* getAgentPlugin()
    {
        return agent_plugin;
    };
    bool createAgentPlugin ( agent* a, Parsed_World* parse )
    {
        if ( agent_plugin!=0 )
        {
            ERR ( "already created agent plugin, please check for double calls",NULL )
        }
        else
            agent_plugin=new agent_router ( a ,parse );
        return agent_plugin;
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
    agent_router* agent_plugin;
    agent_router_parser_plugin* parser_plugin;

};



#endif //AGENT_ROUTER_PLUGIN_H
