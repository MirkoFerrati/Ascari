#include "agent_router/agent_router_plugin.h"
//#include "monitor/monitor_plugin.h"
 #include "task_assignment/task_assignment_plugin.h"
// 
#include <vector>
std::vector<abstract_plugin*> createPlugins()
{
std::vector<abstract_plugin*> temp;
if (CONFIG.exists("AGENT_ROUTER") && atoi(CONFIG.getValue("AGENT_ROUTER").c_str()))  temp.push_back(new agent_router_plugin()); 
//temp.push_back(new monitor_plugin()); 
if (CONFIG.exists("TASK_ASSIGNMENT") && atoi(CONFIG.getValue("TASK_ASSIGNMENT").c_str())) temp.push_back(new task_assignment_plugin()); 
  INFO("Plugins available:",NULL);
  for (auto plugin:temp)
  {
    INFO("%s",(*plugin).getType().c_str());  
  }
  return temp;
}