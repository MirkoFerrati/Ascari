#include "formation_control_parser.h"
#include <logog.hpp>

formation_control_parser::formation_control_parser(std::string type)
{

}

bool formation_control_parser::isEnabled()
{
  return true;
}


abstract_parsed_world_plugin* formation_control_parser::parseWorld(const YAML::Node& node)
{

}

abstract_parsed_agent_plugin* formation_control_parser::parseAgent ( const YAML::Node& node)
{

}