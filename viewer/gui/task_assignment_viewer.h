#ifndef TASK_ASSIGNMENT_VIEWER_H
#define TASK_ASSIGNMENT_VIEWER_H

//written by Alessandro Settimi
#include "yaml_parser.h"
#include <boost/circular_buffer.hpp>
//written by Alessandro Settimi

class task_assignment_viewer
{
public:
  
      //written by Alessandro Settimi
    void initialize_tasks(const std::map<std::string,task_assignment_task>& obj_tasks);
    //written by Alessandro Settimi
private:
      //written by Alessandro Settimi    
    std::map<task_assignment_namespace::agent_id,std::vector<double>> initial_pos;
    bool started;
    double old_time;
    std::map<task_assignment_namespace::agent_id,boost::circular_buffer<double>> positions;
    std::map<task_assignment_namespace::task_id,task_assignment_namespace::task> tasks;
    //written by Alessandro Settimi
  
};

#endif // TASK_ASSIGNMENT_VIEWER_H
