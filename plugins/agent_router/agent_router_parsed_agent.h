#ifndef AGENT_ROUTER_PARSED_AGENT_H
#define AGENT_ROUTER_PARSED_AGENT_H

#include <../plugins/abstract_parser_plugin.h>
typedef int target_id;


class agent_router_parsed_agent : public abstract_parsed_agent_plugin
{

public:
    virtual std::string getType(){return "agent_router_parsed_agent";};
    
    std::vector<target_id> target_list;

};

#endif // AGENT_ROUTER_PARSED_AGENT_H
