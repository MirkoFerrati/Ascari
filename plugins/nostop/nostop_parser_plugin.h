#ifndef NOSTOP_PARSER_PLUGIN_H
#define NOSTOP_PARSER_PLUGIN_H

#include <../plugins/abstract_parser_plugin.h>
#include "nostop_agent_plugin.h"

#include <list>

namespace NoStop
{
	class Real2D;
	///	Parser of the Agent
	class Parsed_agent: public abstract_parsed_agent_plugin
	{
	public:
		std::string getType(){return "NoStop::Parsed_agent";};

		std::string m_id;
		std::string m_camera;

		friend class Parser_plugin;
	};

	///	Parser of the world
	class Parsed_world: public abstract_parsed_world_plugin
	{
	public:
		std::string getType() { return "NoStop::Parsed_world"; }

		/// Get world external boundaries
		std::vector<Real2D> getExternalVertices();

		/// Get obstacles boundaries
		std::list< std::vector<Real2D> > getObstaclesVertices();

		/// Reading world external boundaries from configuration file 
		static std::vector<Real2D> createExternalVertexList( const YAML::Node& node, int const& num );

		/// Reading obstacles from configuration file
		static std::list< std::vector<Real2D> > createObstaclesVertexList( 
			const YAML::Node& node,
			int const& num,
			std::vector<int> const& numVertInObs );

		/// Reading number of vertex for every obstacles from configuration file
		static std::vector<int> readingObstaclesVertices( 
			const YAML::Node& node,
			int const& l_numObs);

		/// Get discretization factor
		int getDiscretization();

	public:

		std::vector<std::string> m_agents;

	protected:
		std::vector<Real2D> m_external;
		std::list< std::vector<Real2D> > m_obstacles;
		int m_numDiscretization;

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
