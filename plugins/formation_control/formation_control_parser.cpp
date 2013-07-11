#include <logog.hpp>

#include "formation_control_parser.h"


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


abstract_parsed_agent_plugin* formation_control_parser::parseAgent(const YAML::Node& node)
{
  abstract_parsed_agent_plugin* ag = new formation_control_parsed_agent();
  
  if(node.FindValue("FINAL"))
  {
    const YAML::Node& co = node["FINAL"];
    double x_final;
    double y_final;
    double theta_final;
    
    co[0] >> x_final;
    co[1] >> y_final;
    co[2] >> theta_final;
    
    reinterpret_cast<formation_control_parsed_agent*>(ag)->final_state[STATE_X] = x_final;
    reinterpret_cast<formation_control_parsed_agent*>(ag)->final_state[STATE_Y] = y_final;
    reinterpret_cast<formation_control_parsed_agent*>(ag)->final_state[STATE_THETA] = theta_final;
    
    return ag;
  }
  else
    return 0;
}