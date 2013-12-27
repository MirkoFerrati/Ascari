#ifndef ABSTRACT_AGENT_PLUGIN_H
#define ABSTRACT_AGENT_PLUGIN_H

#include "exprtk.hpp"
#include <yaml_parser.h>
#include <vector>

class agent;

class abstract_agent_plugin
{
public:
    virtual void run_plugin()=0;
    virtual ~abstract_agent_plugin(){};
    virtual bool initialize(){return true;};
};


class abstract_agent_plugin_container
{
public:
    virtual std::string getType()=0;
    virtual abstract_agent_plugin* getAgentPlugin()=0;
    virtual bool createAgentPlugin ( agent* a, Parsed_World* parse )=0;
    virtual bool createParserPlugin()=0;
    virtual abstract_parser_plugin* getParserPlugin()=0;
    virtual bool isEnabled()=0;
};


#endif // ABSTRACT_AGENT_PLUGIN_H
