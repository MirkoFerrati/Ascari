#ifndef MONITOR_PLUGIN_H
#define MONITOR_PLUGIN_H


#include "../abstract_plugin.h"
#include "monitor_viewer.h"
#include "monitor_parser_plugin.h"
#include "identifier_module.h"
#include <logog.hpp>
#ifdef ISSIMULATOR
class monitor_simulator_plugin: public abstract_simulator_plugin
{
    virtual bool initialize ( const Parsed_World& w )
    {
        return true;
    };
    virtual void run_plugin() {};
    virtual void stop() {};
};

#endif

class monitor_plugin: public abstract_plugin
{
public:
    monitor_plugin()
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

     std::string getType(){return "MONITOR";};

    
#ifdef ISAGENT
    bool createAgentPlugin ( agent* a, Parsed_World* parse )
    {
        if ( agent_plugin!=0 )
            ERR ( "already created agent plugin, please check for double calls",NULL )
        else
            agent_plugin=new identifier_module ( a,parse );
        return agent_plugin;
    };
    abstract_agent_plugin* getAgentPlugin()
    {
        return agent_plugin;
    };
#endif


    bool createParserPlugin()
    {
        if ( parser_plugin!=0 )
	 ERR ( "already created agent plugin, please check for double calls",NULL )
        else
            parser_plugin=new monitor_parser_plugin ( );
        return parser_plugin;
    };
    abstract_parser_plugin* getParserPlugin()
    {
        return parser_plugin;
    };

#ifdef ISSIMULATOR
    bool createSimulatorPlugin ( simulator* s )
    {
        return false;
    };
    abstract_simulator_plugin* getSimulatorPlugin()
    {
      assert(0);//should never be called
    };
#endif

#ifdef ISVIEWER

    bool createViewerPlugin ( Viewer* v )
    {
        if ( viewer_plugin!=0 )
            ERR ( "already created viewer plugin, please check for double calls",NULL )
        else
            viewer_plugin=new monitor_viewer ( v );
        return viewer_plugin;
    };
    abstract_viewer_plugin* getViewerPlugin()
    {
        return viewer_plugin;
    };
#endif

private:
#ifdef ISAGENT
    identifier_module* agent_plugin;
#endif

#ifdef ISVIEWER
    monitor_viewer* viewer_plugin;
#endif

#ifdef ISSIMULATOR
    monitor_simulator_plugin* simulator_plugin;
#endif

    monitor_parser_plugin* parser_plugin;

};



#endif //AGENT_ROUTER_PLUGIN_H
