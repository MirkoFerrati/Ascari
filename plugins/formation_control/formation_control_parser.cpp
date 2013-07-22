#include <logog.hpp>

#include "formation_control_parser.h"


formation_control_parser::formation_control_parser(std::string type)
{}


bool formation_control_parser::isEnabled()
{
  return true;
}


abstract_parsed_world_plugin* formation_control_parser::parseWorld(const YAML::Node& node)
{}


abstract_parsed_agent_plugin* formation_control_parser::parseAgent(const YAML::Node& node)
{
  abstract_parsed_agent_plugin* ag = new formation_control_parsed_agent();
  
  if(node.FindValue("LEADER"))
  {
    const YAML::Node& co = node["LEADER"];
    std::string leader;
    co[0] >> leader;
    reinterpret_cast<formation_control_parsed_agent*>(ag)->leader = leader;
  }
  else
    return 0;
  
  if(node.FindValue("WINGMEN"))
  {
    const YAML::Node& co = node["WINGMEN"];
    std::string tmp;
     
    for(unsigned int i = 0; i < co.size(); i++)
    {
      // CONTROLLO SE GLI AGENTI ELENCATI ESISTONO???
      co[i] >> tmp;
      reinterpret_cast<formation_control_parsed_agent*>(ag)->wingmen.push_back(tmp);
    }
  }
  else
    return 0;
   
  return ag;
}