#ifndef AVOIDOBSTACLES_PARSER_PLUGIN_H
#define AVOIDOBSTACLES_PARSER_PLUGIN_H
#include "../abstract_parser_plugin.h"

class avoidobstacles_parsed_agent: public abstract_parsed_agent_plugin
{
public:
    virtual std::string getType(){return "AVOID OBSTACLES AGENT";};
};

class avoidobstacles_parsed_world: public abstract_parsed_world_plugin
{
    virtual std::string getType(){return "AVOID OBSTACLES WORLD";};
};

class avoidobstacles_parser_plugin: public abstract_parser_plugin
{
public:
    avoidobstacles_parser_plugin(std::string);
    virtual bool isEnabled(){return enabled;};
    virtual std::string getType(){return type;};
    virtual abstract_parsed_agent_plugin* parseAgent(const YAML::Node& node);
    virtual abstract_parsed_world_plugin* parseWorld(const YAML::Node& node);
private:
  bool enabled=false;
  std::string type;
};

#endif // AVOIDOBSTACLES_PARSER_PLUGIN_H
