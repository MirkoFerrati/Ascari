#include <vector>
#include "abstract_agent_plugin.h"
#include "agent_router/agent_router_simulator_plugin.h"
#include <define.h>
std::vector<abstract_simulator_plugin_container*> createSimulatorPlugins()
{
    std::vector<abstract_simulator_plugin_container*> temp;
    if (CONFIG.exists("AGENT_ROUTER") && atoi(CONFIG.getValue("AGENT_ROUTER").c_str()))  temp.push_back(new simulator_router_plugin()); 
    //if (CONFIG.exists("AVOID_OBSTACLES") && atoi(CONFIG.getValue("AVOID_OBSTACLES").c_str()))  temp.push_back(new avoidobstacles_plugin(a, parse)); 
    //if (CONFIG.exists("TASK_ASSIGNMENT") && atoi(CONFIG.getValue("TASK_ASSIGNMENT").c_str())) temp.push_back(new task_assignment_plugin(a, parse)); 
    INFO("Plugins available:",NULL);
    for (auto plugin:temp)
    {
        INFO("%s",(*plugin).getType().c_str());  
    }
    return temp;
}