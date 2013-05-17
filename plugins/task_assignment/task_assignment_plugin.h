#ifndef TASK_ASSIGNMENT_PLUGIN_H
#define TASK_ASSIGNMENT_PLUGIN_H
#include "../plugins/abstract_plugin.h"
#include "task_assignment.h"
#include "task_assignment_parser_plugin.h"
#include "task_assignment_viewer.h"


class task_assignment_plugin: public abstract_plugin
{
public:
    task_assignment_plugin()
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
            agent_plugin=new task_assignment( a ,parse);
        return agent_plugin;
    };
#endif

 std::string getType(){return "TASK_ASSIGNMENT";};




#ifdef ISSIMULATOR
    abstract_simulator_plugin* getSimulatorPlugin()
    {
        return simulator_plugin;
    };
    bool createSimulatorPlugin ( simulator* s )
    {
        if ( simulator_plugin!=0 )
            ERR ( "already created simulator plugin, please check for double calls",NULL )
        else
            simulator_plugin=new task_assignment_simulator_plugin ( s );
        return simulator_plugin;
    };
#endif
    
#ifdef ISVIEWER
    abstract_viewer_plugin* getViewerPlugin()
    {
        return viewer_plugin;
    };
    bool createViewerPlugin ( Viewer* v )
    {
        if ( viewer_plugin!=0 )
            ERR ( "already created viewer plugin, please check for double calls",NULL )
        else
            viewer_plugin=new task_assignment_viewer ( v );
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
            parser_plugin=new task_assignment_parser_plugin ( );
        return parser_plugin;
    };



private:
#ifdef ISAGENT
    task_assignment* agent_plugin;
#endif
    
#ifdef ISVIEWER
   task_assignment_viewer* viewer_plugin;
#endif
    
#ifdef ISSIMULATOR
    task_assignment_simulator_plugin* simulator_plugin;
#endif

    task_assignment_parser_plugin* parser_plugin;

};






#endif //TASK_ASSIGNMENT_PLUGIN_H