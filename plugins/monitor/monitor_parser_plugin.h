#ifndef MONITOR_PARSER_PLUGIN_H
#define MONITOR_PARSER_PLUGIN_H
#include "../abstract_parser_plugin.h"



class monitor_parser_plugin : public abstract_parser_plugin
{

public:
    virtual abstract_parsed_world_plugin* parseWorld ( const YAML::Node& node);
    virtual abstract_parsed_agent_plugin* parseAgent ( const YAML::Node& node);
};

#endif // MONITOR_PARSER_PLUGIN_H
