#ifndef AGENT_ROUTER_PARSER_PLUGIN_H
#define AGENT_ROUTER_PARSER_PLUGIN_H

#include <objects/router_graph_good.h>
#include "../abstract_parser_plugin.h"


class agent_router_parser_plugin : public abstract_parser_plugin
{

public:
    virtual std::shared_ptr< abstract_parsed_world_plugin > parseWorld ( const YAML::Node& node );
    virtual std::shared_ptr< abstract_parsed_agent_plugin > parseAgent ( const YAML::Node& node );
    
private:
    std::map<int,router_graph_good> goods;

};

#endif // AGENT_ROUTER_PARSER_PLUGIN_H
