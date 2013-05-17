#ifndef AGENT_ROUTER_PARSER_PLUGIN_H
#define AGENT_ROUTER_PARSER_PLUGIN_H

#include <objects/router_graph_good.h>
#include "../abstract_parser_plugin.h"


class agent_router_parser_plugin : public abstract_parser_plugin
{

public:
    virtual  abstract_parsed_world_plugin* parseWorld ( const YAML::Node& node);
    virtual bool parseAgent ( const YAML::Node& node,abstract_parsed_agent_plugin* );
    
private:
    std::map<int,router_graph_good> goods;
    bool enabled;

};

#endif // AGENT_ROUTER_PARSER_PLUGIN_H
