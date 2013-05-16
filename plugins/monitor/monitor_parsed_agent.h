#ifndef MONITOR_PARSED_AGENT_H
#define MONITOR_PARSED_AGENT_H

#include <../../plugins/abstract_parser_plugin.h>


class monitor_parsed_agent : public abstract_parsed_agent_plugin
{

public:
    virtual std::string getType(){return "monitor_parsed_agent";};
        
    std::vector<std::string> known_behaviors;
        bool monitoring;
    
};

#endif // MONITOR_PARSED_AGENT_H
