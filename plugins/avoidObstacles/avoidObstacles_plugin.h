#ifndef AVOID_OBSTACLES_PLUGIN_H
#define AVOID_OBSTACLES_PLUGIN_H
#include "../abstract_plugin.h"
#include "avoidobstacles_parser_plugin.h"


#ifdef ISAGENT
#include "avoidobstacles_agent_plugin.h"
#endif

class avoidobstacles_plugin: public abstract_plugin
{
public:
    avoidobstacles_plugin()
    {
#ifdef ISAGENT
        agent_plugin=0;
#endif
        parser_plugin=0;
    };


    std::string getType()
    {
        return "AVOID_OBSTACLES";
    };


#ifdef ISAGENT
    abstract_agent_plugin* getAgentPlugin()
    {
        return agent_plugin;
    };
    bool createAgentPlugin ( agent* a, Parsed_World*)
    {
        if ( agent_plugin!=0 )
        {
            ERR ( "already created agent plugin, please check for double calls",NULL );
        }
        else
            agent_plugin=new avoidObstacles_agent_plugin( a);
        return agent_plugin;
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
            ERR ( "already created agent plugin, please check for double calls",NULL );
        }
        else
            parser_plugin=new avoidobstacles_parser_plugin ( getType());
        return parser_plugin;
    };

#ifdef ISSIMULATOR
    virtual abstract_simulator_plugin* getSimulatorPlugin(){return 0;};
#endif

#ifdef ISVIEWER
    virtual abstract_viewer_plugin* getViewerPlugin(){return 0;};
#endif

private:
#ifdef ISAGENT
    avoidObstacles_agent_plugin* agent_plugin;
#endif

    
    
    avoidobstacles_parser_plugin* parser_plugin;

};

#endif