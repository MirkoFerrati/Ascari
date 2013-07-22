#ifndef FORMATION_CONTROL_PARSER_H
#define FORMATION_CONTROL_PARSER_H

#include <../plugins/abstract_parser_plugin.h>
#include "formation_control_types.h"


class formation_control_parsed_agent: public abstract_parsed_agent_plugin
{
public:
  std::string leader;
  std::vector<std::string> wingmen;
  
  std::string getType()
  {
    return "formation_control_parsed_agent";
  };
};


class formation_control_parsed_world: public abstract_parsed_world_plugin
{
public:
    std::string getType()
    {
      return "formation_control_parsed_world";
    };
};


class formation_control_parser : public abstract_parser_plugin
{
public:
  formation_control_parser(std::string type);
  
  abstract_parsed_world_plugin* parseWorld(const YAML::Node& node);
  abstract_parsed_agent_plugin* parseAgent(const YAML::Node& node);
  
  bool isEnabled();
  
  std::string getType()
  {
    return "Formation Control Plugin";
  };
};

#endif // FORMATION_CONTROL_PARSER_H
