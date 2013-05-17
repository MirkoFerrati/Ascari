#include "agent_router/agent_router_plugin.h"
#include "monitor/monitor_plugin.h"
#include "task_assignment/task_assignment_plugin.h"

#include <vector>

std::vector<abstract_plugin*> createPlugins()
{
 std::vector<abstract_plugin*> temp;
temp.push_back(new agent_router_plugin()); 
temp.push_back(new monitor_plugin()); 
temp.push_back(new task_assignment_plugin()); 
  return temp;
}