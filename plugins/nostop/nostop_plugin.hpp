#ifndef NOSTOP_PLUGIN_H
#define NOSTOP_PLUGIN_H
#define NOSTOP_PLUGIN_IDENTIFIER "NOSTOP"

#include "../plugins/abstract_plugin.h"
#include "nostop_parser_plugin.h"

#ifdef ISAGENT
#include "nostop_agent_plugin.h"
#endif

#ifdef ISVIEWER
#include "nostop_viewer_plugin.h"
#endif

#ifdef ISSIMULATOR
#include "nostop_simulator_plugin.h"
#endif

namespace NoStop
{
	///	Abstract Base Plugin
	class Base_plugin: public abstract_plugin
	{
	private:
#ifdef ISAGENT
		Agent_plugin* m_agent_plugin;
#endif

#ifdef ISVIEWER
		Viewer_plugin* m_viewer_plugin;
#endif

#ifdef ISSIMULATOR
		Simulator_plugin* m_simulator_plugin;
#endif

		Parser_plugin* m_parser_plugin;

	public:
		Base_plugin()
		{
#ifdef ISAGENT
			m_agent_plugin=0;
#endif
#ifdef ISSIMULATOR
			m_simulator_plugin=0;
#endif
#ifdef ISVIEWER
			m_viewer_plugin=0;
#endif
			m_parser_plugin=0;
		};

		///	Type of the plugin
		std::string getType()
		{
			return NOSTOP_PLUGIN_IDENTIFIER;
		};

#ifdef ISAGENT
		abstract_agent_plugin* getAgentPlugin()
		{
			return m_agent_plugin;
		};
		bool createAgentPlugin ( agent* a, Parsed_World* parse )
		{
			if ( m_agent_plugin!=0 )
			{
				ERR ( "Already created agent plugin, please check for double calls",NULL );
			}
			else
				m_agent_plugin=new NoStop::Agent_plugin ( a ,parse );
			return m_agent_plugin;
		};
#endif

#ifdef ISSIMULATOR
		abstract_simulator_plugin* getSimulatorPlugin()
		{
			return m_simulator_plugin;
		};
		bool createSimulatorPlugin ( simulator* s ,Parsed_World* world )
		{
			if ( m_simulator_plugin!=0 )
				ERR ( "already created simulator plugin, please check for double calls",NULL );
			else
				m_simulator_plugin=new NoStop::Simulator_plugin ( s );
			return m_simulator_plugin;
		};
#endif

#ifdef ISVIEWER
		abstract_viewer_plugin* getViewerPlugin()
		{
			return m_viewer_plugin;
		};
		bool createViewerPlugin ( Viewer* v,Parsed_World* parse=0 )
		{
			if ( m_viewer_plugin!=0 )
				ERR ( "Already created viewer plugin, please check for double calls",NULL );
			else
				m_viewer_plugin=new NoStop::Viewer_plugin( );
			return m_viewer_plugin;
		};
#endif


		abstract_parser_plugin* getParserPlugin()
		{
			return m_parser_plugin;
		};

		bool createParserPlugin()
		{
			if ( m_parser_plugin!=0 )
				ERR ( "Already created agent plugin, please check for double calls",NULL )
			else
			m_parser_plugin=new Parser_plugin ( getType() );
			return m_parser_plugin;
		};

	};
}

#endif //NOSTOP_PLUGIN_H
