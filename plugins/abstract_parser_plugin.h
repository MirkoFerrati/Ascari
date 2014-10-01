#ifndef ABSTRACT_PARSER_PLUGIN_H
#define ABSTRACT_PARSER_PLUGIN_H
#include <yaml-cpp/node/node.h>


class abstract_parsed_agent_plugin
{
public:
  abstract_parsed_agent_plugin(){};
  
  virtual std::string getType()=0;
  virtual ~abstract_parsed_agent_plugin(){};

};

class abstract_parsed_world_plugin
{
public:
  abstract_parsed_world_plugin(){};
  virtual std::string getType()=0;
  virtual ~abstract_parsed_world_plugin(){};
};


class abstract_parser_plugin
{
public:
  virtual abstract_parsed_agent_plugin* parseAgent(const YAML::Node& node)=0;
  virtual abstract_parsed_world_plugin* parseWorld(const YAML::Node& node)=0;
  virtual ~abstract_parser_plugin(){};
  bool isCreated=true;
  virtual bool isEnabled()=0;
  virtual std::string getType()=0;

};

#endif // ABSTRACT_PARSER_PLUGIN_H
