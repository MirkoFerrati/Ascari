#ifndef AVOIDOBSTACLES_PARSER_PLUGIN_H
#define AVOIDOBSTACLES_PARSER_PLUGIN_H
#include "../abstract_parser_plugin.h"

class avoidobstacles_parsed_agent: public abstract_parsed_agent_plugin
{
  
};

class avoidobstacles_parsed_world: public abstract_parsed_world_plugin
{
};

class avoidobstacles_parser_plugin: public abstract_parser_plugin
{
public:
    avoidobstacles_parser_plugin();
    virtual std::string getType(){return type;};
    virtual abstract_parsed_agent_plugin* parseAgent(const YAML::Node& node);
    virtual abstract_parsed_world_plugin* parseWorld(const YAML::Node& node);
private:
  std::string type;
};

#endif // AVOIDOBSTACLES_PARSER_PLUGIN_H
