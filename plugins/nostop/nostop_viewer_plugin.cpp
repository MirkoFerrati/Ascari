#ifndef NOSTOP_VIEWER_H
#define NOSTOP_VIEWER_H
#include "../plugins/abstract_viewer_plugin.h"

#ifdef ISVIEWER

#include "yaml_parser.h"
#include <boost/circular_buffer.hpp>
#include "objects/task_assignment_task.h"
#include "viewer.h"

namespace NoStop
{
  class Viewer_plugin : public abstract_viewer_plugin
  {
  public:
      Viewer_plugin();
      Viewer_plugin ( std::shared_ptr< std::mutex >& mutex, const world_sim_packet& infos );
      void timerEvent(std::shared_ptr<std::mutex>& mutex,const world_sim_packet& infos);
      void paintBackground ( QPainter& painter );
      void paintAgents ( QPainter& painter,const std::map<std::string,Agent>& agents );  
      ~Viewer_plugin();

  private:
      std::map<task_assignment_namespace::agent_id,std::vector<double>> initial_pos;
      bool started;
      double old_time,now_time;
      //const int& time;
      std::map<task_assignment_namespace::agent_id,boost::circular_buffer<double>> positions;
      std::map<task_assignment_namespace::task_id,task_assignment_namespace::task> tasks;
    // const std::shared_ptr< std::mutex >& mutex;
    // const world_sim_packet& infos;

  };
}

#endif
#endif // TASK_ASSIGNMENT_VIEWER_H
