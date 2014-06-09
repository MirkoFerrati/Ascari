//written by Alessandro Settimi
#ifndef TASK_ASSIGNMENT_VIEWER_H
#define TASK_ASSIGNMENT_VIEWER_H
#include "../plugins/abstract_viewer_plugin.h"

#include "../HACK_KDEVELOP.h"
#ifdef ISVIEWER

#include "yaml_parser.h"
#include <boost/circular_buffer.hpp>
#include "objects/task_assignment_task.h"
#include "viewer.h"

struct task_viewer_agent
{
        QGraphicsEllipseItem* initial_pos;
        boost::circular_buffer<QGraphicsRectItem*> positions;
        QGraphicsTextItem* charge;
        boost::circular_buffer<QGraphicsRectItem*>::iterator it;
};


class task_assignment_viewer:public abstract_viewer_plugin
{
public:
    task_assignment_viewer();
    task_assignment_viewer ( std::shared_ptr< std::mutex >& mutex, const world_sim_packet& infos );
    void timerEvent(std::shared_ptr<std::mutex>& mutex,const world_sim_packet& infos);
    void paintBackground ( QGraphicsScene* scene );
    void paintAgents ( QGraphicsScene* scene, std::map<std::string,Agent>& agents );  
    ~task_assignment_viewer();

private:
    std::map<task_assignment_namespace::agent_id,task_viewer_agent> my_agents;
    bool started;
    double old_time,now_time;
    std::map<task_assignment_namespace::task_id,task_assignment_namespace::task> tasks;
    std::map<task_assignment_namespace::task_id,QGraphicsRectItem*> viewer_tasks;
    std::map<task_assignment_namespace::task_id,QGraphicsTextItem*> viewer_text_tasks;
    std::map<task_assignment_namespace::task_id,QGraphicsTextItem*> viewer_deadline_tasks;
    
    double size;

};

#endif
#endif // TASK_ASSIGNMENT_VIEWER_H
