#ifndef AGENT_ROUTER_PLUGIN_H
#define AGENT_ROUTER_PLUGIN_H
#include "../abstract_plugin.h"
#include "agent_router_parser_plugin.h"


#ifdef ISAGENT
#include "agent_router.h"
#endif

#ifdef ISVIEWER
#include "agent_router_viewer.h"
#endif

#ifdef ISSIMULATOR
#include "agent_router_simulator_plugin.h"
#endif




class agent_router_plugin: public abstract_plugin
{
public:
    agent_router_plugin()
    {
#ifdef ISAGENT
        agent_plugin=0;
#endif
#ifdef ISSIMULATOR
        simulator_plugin=0;
#endif
#ifdef ISVIEWER
        viewer_plugin=0;
#endif
        parser_plugin=0;
    };


    std::string getType()
    {
        return "AGENT_ROUTER";
    };


#ifdef ISAGENT
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
#endif





#ifdef ISSIMULATOR
    abstract_simulator_plugin* getSimulatorPlugin()
    {
        return simulator_plugin;
    };
    bool createSimulatorPlugin ( simulator* s,Parsed_World* parse )
    {
        if ( simulator_plugin!=0 )
            ERR ( "already created agent plugin, please check for double calls",NULL )
            else
            {
                simulator_plugin=new agent_router_simulator_plugin ( s );
            }
        return simulator_plugin;
    };
#endif

#ifdef ISVIEWER
    abstract_viewer_plugin* getViewerPlugin()
    {
        return viewer_plugin;
    };
    bool createViewerPlugin ( Viewer* v, Parsed_World* parse )
    {
        if ( viewer_plugin!=0 )
            ERR ( "already created viewer plugin, please check for double calls",NULL )
            else
                viewer_plugin=new agent_router_viewer ( parse);
        return viewer_plugin;
    };
#endif


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
            parser_plugin=new agent_router_parser_plugin ( );
        return parser_plugin;
    };



private:
#ifdef ISAGENT
    agent_router* agent_plugin;
#endif

#ifdef ISVIEWER
    agent_router_viewer* viewer_plugin;
#endif

#ifdef ISSIMULATOR
    agent_router_simulator_plugin* simulator_plugin;
#endif

    agent_router_parser_plugin* parser_plugin;

};



#endif //AGENT_ROUTER_PLUGIN_H
