#ifndef TASK_ASSIGNMENT_PARSER_PLUGIN_H
#define TASK_ASSIGNMENT_PARSER_PLUGIN_H

#include <../plugins/abstract_parser_plugin.h>
#include "task_assignment_types.h"


class task_assignment_parsed_agent: public abstract_parsed_agent_plugin
{
public:
      task_assignment_namespace::task_cost_vector agent_task_cost_vector;
    std::string getType(){return "task_assignment_parsed_agent";};

};
class task_assignment_parsed_world: public abstract_parsed_world_plugin
{
public:
    task_assignment_namespace::task_assignment_algorithm task_assignment_algorithm;
  task_assignment_namespace::task_list task_list;
    std::vector<task_assignment_namespace::task_id> tasks_id;
    std::string getType(){return "task_assignment_parsed_world";};
std::vector<std::string> agents;
};
class task_assignment_parser_plugin : public abstract_parser_plugin
{

public:
  task_assignment_parser_plugin();
    bool parseWorld ( const YAML::Node& node,abstract_parsed_world_plugin* wo );
    bool parseAgent ( const YAML::Node& node,abstract_parsed_agent_plugin* ag );
    
private:
  //written by Alessandro Settimi
void createTaskList ( const YAML::Node& node, task_assignment_namespace::task_list& task_list,std::vector<task_assignment_namespace::task_id>& tasks_id,
                      unsigned int task_number );
    bool enabled;

};

#endif // TASK_ASSIGNMENT_PARSER_PLUGIN_H
