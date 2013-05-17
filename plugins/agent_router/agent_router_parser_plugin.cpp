#include "agent_router_parser_plugin.h"
#include "agent_router_parsed_agent.h"
#include "agent_router_parsed_world.h"

#include <logog.hpp>

bool agent_router_parser_plugin::parseWorld ( const YAML::Node& node, abstract_parsed_world_plugin* wo )
{
    assert ( wo==0 );
    enabled=false;
    wo=new agent_router_parsed_world();
    reinterpret_cast<agent_router_parsed_world*> ( wo )->graphName="UNSET";
    if ( node[0]["WORLD"][0].FindValue ( "GRAPH_NAME" ) )
    {
        const YAML::Node &world_node=node[0]["WORLD"][0];

        world_node["GRAPH_NAME"]>> ( reinterpret_cast<agent_router_parsed_world*> ( wo ) )->graphName;
        enabled=true;
    }
    return enabled;
}

bool agent_router_parser_plugin::parseAgent ( const YAML::Node& node, abstract_parsed_agent_plugin* ag )
{
  assert(ag==0);
    if ( !enabled )
        return false;
    ag=new agent_router_parsed_agent();
    if ( node.FindValue ( "TARGET_LIST" ) )
    {
        int toint=0;
        std::vector<target_id>temp = ( reinterpret_cast<agent_router_parsed_agent*> ( ag ) )->target_list;
        for ( unsigned int i=0; i<node["TARGET_LIST"].size(); i++ )
        {
            node["TARGET_LIST"][i]>>toint;
            temp.push_back ( toint );
        }
    }
    if ( ( reinterpret_cast<agent_router_parsed_agent*> ( ag )->target_list.size() ==0 && enabled ) || ( reinterpret_cast<agent_router_parsed_agent*> ( ag )->target_list.size() >0 && !enabled ) )
    {
        ERR ( "GRAPH NAME OR TARGET LIST UNDEFINED", NULL );
        return false;
    }
    return true;
}

