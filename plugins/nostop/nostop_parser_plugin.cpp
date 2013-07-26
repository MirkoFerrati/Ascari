#include "nostop_parser_plugin.h"
#include "nostop_real2D.h"

#include <set>
#include <vector>

#include <logog.hpp>

namespace NoStop
{
	//////////////////////////////////////////////////////////////////////////
	Parser_plugin::Parser_plugin(std::string type)
		:	m_type(type), m_enabled(false)
	{}

	//////////////////////////////////////////////////////////////////////////
	abstract_parsed_world_plugin* Parser_plugin::parseWorld ( const YAML::Node& node)
	{
		Parsed_world* wo	=	new Parsed_world();

		// Collect number of agents
		const YAML::Node &agent_nodes=node[0]["AGENTS"];
		wo->m_agents.reserve ( agent_nodes.size() );
		for ( unsigned int i=0; i<agent_nodes.size(); ++i )
		{
			std::string tmp_ag_name;
			agent_nodes[i]["AGENT"]>>tmp_ag_name;
			wo->m_agents.push_back(tmp_ag_name); 
		}

		// Collect external environments information
		if ( node[0].FindValue ( "WORLD" ) )
		{
			if ( node[0]["WORLD"].size() >0 )
			{
				if( node[0]["WORLD"][0].FindValue ( "NOSTOP_AREA" ) && 
					node[0]["WORLD"][0].FindValue ( "NOSTOP_DISCRETIZATION" ) )
				{
					m_enabled=true;

					if( node[0]["WORLD"][0]["NOSTOP_AREA"][0].find("NOSTOP_DISCRETIZATION") )
					{
						node[0]["WORLD"][0]["NOSTOP_AREA"][0]["NOSTOP_DISCRETIZATION"] >> wo->m_numDiscretization;
					}

					if( node[0]["WORLD"][0]["NOSTOP_AREA"][0].find("NOSTOP_NUMBER_OF_VERTEX_EXTERNAL") &&
						node[0]["WORLD"][0]["NOSTOP_AREA"][0].find("NOSTOP_VERTICES_EXTERNAL") )
					{
						int l_numVert = 0;
						node[0]["WORLD"][0]["NOSTOP_AREA"][0]["NOSTOP_NUMBER_OF_VERTEX_EXTERNAL"] >> l_numVert;

						wo->m_external = Parsed_world::createExternalVertexList( node[0]["WORLD"][0]["NOSTOP_VERTICES_EXTERNAL"], l_numVert );
					}

					if( node[0]["WORLD"][0]["NOSTOP_AREA"][0].find("NOSTOP_NUMBER_OF_OBSTACLES") &&
						node[0]["WORLD"][0]["NOSTOP_AREA"][0].find("NOSTOP_NUMBER_OF_VERTEX_OBSTACLES") &&
						node[0]["WORLD"][0]["NOSTOP_AREA"][0].find("NOSTOP_VERTICES_OBSTACLES"))
					{
						int l_numObs = 0;
						node[0]["WORLD"][0]["NOSTOP_AREA"][0]["NOSTOP_NUMBER_OF_OBSTACLES"] >> l_numObs;

						std::vector<int> l_numVertInObs = Parsed_world::readingObstaclesVertices( 
							node[0]["WORLD"][0]["NOSTOP_NUMBER_OF_VERTEX_OBSTACLES"], l_numObs );

						wo->m_obstacles = Parsed_world::createObstaclesVertexList( 
							node[0]["WORLD"][0]["NOSTOP_VERTICES_OBSTACLES"], 
							l_numObs, 
							l_numVertInObs );
					}
				}
			}
		}
		return m_enabled?wo:NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	abstract_parsed_agent_plugin* Parser_plugin::parseAgent ( const YAML::Node& node)
	{
		//   assert(ag==0);
		if (!m_enabled)
			return 0;

		if ( node.FindValue ( "NOSTOP_ID" ) &&
			 node.FindValue( "NOSTOP_CAMERA" ) )
		{
			Parsed_agent* ag	=	new Parsed_agent();
			node["NOSTOP_ID"] >> ag->m_id;
			node["NOSTOP_CAMERA"] >> ag->m_camera;
			
			return ag;
		}
		else
			return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	std::vector<Real2D> Parsed_world::createExternalVertexList( 
		const YAML::Node& node, 
		int const& num_vertex )
	{
		std::vector<Real2D> result;
		result.resize(num_vertex);
		for ( unsigned int i=0; i<num_vertex; ++i )
		{
			node[2*i + 0] >> result[i][0];
			node[2*i + 1] >> result[i][1];
		}
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	std::vector<int> Parsed_world::readingObstaclesVertices( 
		const YAML::Node& node, int const& l_numObs)
	{
		std::vector<int> result;
		result.resize(l_numObs);
		for ( unsigned int i=0; i<l_numObs; ++i )
			node[i] >> result[i];
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	std::set< std::vector<Real2D> > Parsed_world::createObstaclesVertexList( 
		const YAML::Node& node, 
		int const& numObs,
		std::vector<int> const& numVertInObs)
	{
		int index = 0;
		std::set< std::vector<Real2D> > result;
		for(unsigned int i = 0; i < numObs; ++i)
		{
			std::vector<Real2D> l_bound;
			l_bound.resize(numVertInObs[i]);
			for ( unsigned int j=0; j < numVertInObs[i]; ++j )
			{
				Real2D l_pos;
				node[2*index + 0] >> l_bound[j][0];
				node[2*index + 1] >> l_bound[j][1];
				index++;
			}
			result.insert( l_bound );
		}
		return result;
	}
}