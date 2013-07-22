#ifndef NOSTOP_PARSER_PLUGIN_H
#define NOSTOP_PARSER_PLUGIN_H

#include <../plugins/abstract_parser_plugin.h>
#include "nostop_agent_plugin.h"

namespace NoStop
{
	///	Parser of the Agent
	class Parsed_agent: public abstract_parsed_agent_plugin
	{
	public:
		std::string getType(){return "NoStop_parsed_agent";};
		
		friend class Parser_plugin;
	};

	///	Parser of the world
	class Parsed_world: public abstract_parsed_world_plugin
	{
	public:
		std::string getType() { return "NoStop_parsed_world"; }

		/// Get world external boundaries
		std::vector<Real2D> getExternalVertices();

		/// Get obstacles boundaries
		std::set< std::vector<Real2D> > getObstaclesVertices();

		/// Reading world external boundaries from configuration file 
		static std::vector<Real2D> createExternalVertexList( const YAML::Node& node, int const& num );
		
		/// Reading obstacles from configuration file
		static std::set< std::vector<Real2D> > createObstaclesVertexList( 
			const YAML::Node& node,
			int const& num,
			std::vector<int> const& numVertInObs );

		/// Reading number of vertex for every obstacles from configuration file
		static std::vector<int> Parsed_world::readingObstaclesVertices( 
			const YAML::Node& node,
			int const& l_numObs);

		/// Get discretization factor
		int getDiscretization();

	protected:
		std::vector<Real2D> m_external;
		std::set< std::vector<Real2D> > m_obstacles;
		int m_numDiscretization;
		
		std::vector<std::string> m_agents;

		friend class Parser_plugin;
	};

	///	Parser: read configuration file
	class Parser_plugin : public abstract_parser_plugin
	{
	public:
		Parser_plugin(std::string type);
		abstract_parsed_world_plugin* parseWorld ( const YAML::Node& node);
		abstract_parsed_agent_plugin* parseAgent ( const YAML::Node& node);
		bool isEnabled() {return m_enabled;}
		std::string getType(){return m_type;}

	private:
		std::string m_type;
		bool m_enabled;
	};
}

#endif // NOSTOP_PARSER_PLUGIN_H
