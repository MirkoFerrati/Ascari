#include "avoidobstacles_parser_plugin.h"

avoidobstacles_parser_plugin::avoidobstacles_parser_plugin(std::string type)
{
  this->type=type;
}


abstract_parsed_agent_plugin* avoidobstacles_parser_plugin::parseAgent(const YAML::Node& node)
{
    enabled=false;
    auto ag=new avoidobstacles_parsed_agent();
    if ( node.FindValue ( "AVOID_OBSTACLES" ) )
    {
      enabled=true;
    }
    return ag;
}

abstract_parsed_world_plugin* avoidobstacles_parser_plugin::parseWorld(const YAML::Node& node)
{
  auto wo= new avoidobstacles_parsed_world();
  return wo;
}


