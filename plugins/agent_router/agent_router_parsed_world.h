#ifndef AGENT_ROUTER_PARSED_WORLD_H
#define AGENT_ROUTER_PARSED_WORLD_H
#include "../abstract_parser_plugin.h"

class agent_router_parsed_world: public abstract_parsed_world_plugin
{
public:
    virtual std::string getType(){return "abstract_parsed_world_plugin";};
          std::string graphName;

};

#endif // AGENT_ROUTER_PARSED_WORLD_H
