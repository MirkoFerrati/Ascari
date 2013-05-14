#include "agent_router_parser_plugin.h"

std::shared_ptr< abstract_parsed_world_plugin > agent_router_parser_plugin::parseWorld ( const YAML::Node& node )
{
    wo.graphName="UNSET";
  if ( node[0]["WORLD"][0].FindValue ( "GRAPH_NAME" ) )
            {
                const YAML::Node &world_node=node[0]["WORLD"][0];

                world_node["GRAPH_NAME"]>> wo.graphName;
            }
}

std::shared_ptr< abstract_parsed_agent_plugin > agent_router_parser_plugin::parseAgent ( const YAML::Node& node )
{
  if ( node.FindValue ( "TARGET_LIST" ) )
    {
        node["TARGET_LIST"]>> ag.target_list;
    }

  
        if ( ( wo.agents[i].target_list.size() ==0 && wo.graphName.compare ( "UNSET" ) !=0 ) || ( wo.agents[i].target_list.size() >0 && wo.graphName.compare ( "UNSET" ) ==0 ) )
        {
            ERR ( "GRAPH NAME OR TARGET LIST UNDEFINED", NULL );
            return false;
        }
}

