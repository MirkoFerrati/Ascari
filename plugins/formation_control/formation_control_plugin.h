#ifndef FORMATION_CONTROL_PLUGIN_H
#define FORMATION_CONTROL_PLUGIN_H
#define FORMATION_CONTROL_PLUGIN_IDENTIFIER "formation_control"

#include "../plugins/abstract_plugin.h"
#include "formation_control_parser.h"

#ifdef ISAGENT
#include "formation_control_agent.h"
#endif

#ifdef ISVIEWER
#include "formation_control_viewer.h"
#endif

#ifdef ISSIMULATOR
#include "formation_control_simulator.h"
#endif

class formation_control_plugin: public abstract_plugin
{
public:
    formation_control_plugin()
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
            agent_plugin = new formation_control_agent( a ,parse );
        return agent_plugin;
    };
#endif

    std::string getType()
    {
        return FORMATION_CONTROL_PLUGIN_IDENTIFIER;
    };




#ifdef ISSIMULATOR
    abstract_simulator_plugin* getSimulatorPlugin()
    {
        return simulator_plugin;
    };
    bool createSimulatorPlugin ( simulator* s ,Parsed_World* world )
    {
        if ( simulator_plugin!=0 )
            ERR ( "already created simulator plugin, please check for double calls",NULL )
            else
                simulator_plugin=new formation_control_simulator ( s );
        return simulator_plugin;
    };
#endif

#ifdef ISVIEWER
    abstract_viewer_plugin* getViewerPlugin()
    {
        return viewer_plugin;
    };
    bool createViewerPlugin ( Viewer* v,Parsed_World* parse=0 )
    {
        if ( viewer_plugin!=0 )
            ERR ( "already created viewer plugin, please check for double calls",NULL )
            else
                viewer_plugin=new formation_control_viewer ( );
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
            ERR ( "already created agent plugin, please check for double calls",NULL )
            else
                parser_plugin=new formation_control_parser( getType());
        return parser_plugin;
    };


private:
#ifdef ISAGENT
    formation_control_agent* agent_plugin;
#endif

#ifdef ISVIEWER
    formation_control_viewer* viewer_plugin;
#endif

#ifdef ISSIMULATOR
    formation_control_simulator* simulator_plugin;
#endif

    formation_control_parser* parser_plugin;

};


#endif //FORMATION_CONTROL_PLUGIN_H
