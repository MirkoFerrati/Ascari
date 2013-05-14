#ifndef ABSTRACT_PARSER_PLUGIN_H
#define ABSTRACT_PARSER_PLUGIN_H
#include <parserYAML/include/yaml-cpp/node.h>


class abstract_parsed_agent_plugin
{
public:
  abstract_parsed_agent_plugin()=delete;
};

class abstract_parsed_world_plugin
{
public:
  abstract_parsed_world_plugin()=delete;
};


class abstract_parser_plugin
{
public:
  virtual std::shared_ptr<abstract_parsed_agent_plugin> parseAgent(const YAML::Node& node)=0;
  virtual std::shared_ptr<abstract_parsed_world_plugin> parseWorld(const YAML::Node& node)=0;
  virtual ~abstract_parser_plugin(){};
  
};

#endif // ABSTRACT_PARSER_PLUGIN_H
