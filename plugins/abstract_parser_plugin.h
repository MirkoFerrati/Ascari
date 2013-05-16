#ifndef ABSTRACT_PARSER_PLUGIN_H
#define ABSTRACT_PARSER_PLUGIN_H
#include <parserYAML/include/yaml-cpp/node.h>


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
  virtual bool parseAgent(const YAML::Node& node,abstract_parsed_agent_plugin*)=0;
  virtual bool parseWorld(const YAML::Node& node,abstract_parsed_world_plugin*)=0;
  virtual ~abstract_parser_plugin(){};
  
};

#endif // ABSTRACT_PARSER_PLUGIN_H
