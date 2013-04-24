//written by Alessandro Settimi
#ifndef TASK_ASSIGNMENT_VIEWER_H
#define TASK_ASSIGNMENT_VIEWER_H
#include "yaml_parser.h"
#include <boost/circular_buffer.hpp>
#include "objects/task_assignment_task.h"
#include "viewer.h"
#include "viewer_plugin.h"


class task_assignment_viewer:public viewer_plugin
{
public:
    task_assignment_viewer (  int* time,std::shared_ptr< std::mutex >& mutex, const world_sim_packet& infos );
    void timerEvent();
    void paintBackground ( QPainter& painter );
    void paintAgents ( QPainter& painter, std::map<std::string,Agent>& agents );
    void initialize_tasks ( const std::map<std::string,task_assignment_task>& obj_tasks );

    ~task_assignment_viewer();

private:
    std::map<task_assignment_namespace::agent_id,std::vector<double>> initial_pos;
    bool started;
    double old_time;
    const int* time;
    std::map<task_assignment_namespace::agent_id,boost::circular_buffer<double>> positions;
    std::map<task_assignment_namespace::task_id,task_assignment_namespace::task> tasks;
    const std::shared_ptr< std::mutex >& mutex;
    const world_sim_packet& infos;

};

#endif // TASK_ASSIGNMENT_VIEWER_H
