#ifndef NOSTOP_SIMULATOR_H
#define NOSTOP_SIMULATOR_H
#ifdef ISSIMULATOR
#include <../plugins/abstract_simulator_plugin.h>
#include "nostop_router.hpp"
#include <yaml_parser.h>
#include <types/world_sim_packet.h>
#include "../simulator/simulator.h"

namespace NoStop
{
  class Simulator_plugin : public abstract_simulator_plugin
  {
  public:
    Simulator_plugin( simulator* s );
	
	/// Get world information and initialize simulator
    bool initialize(Parsed_World const& w);

	/// Run NoStop Simulator plugin
    void run_plugin();

	/// Stop NoStop Simulator plugin
    void stop();

  private:

	  std::shared_ptr<DiscretizedArea> m_area;

    int num_agents;

    bool ta_router_started;
	  
	  task_assignment_namespace::task_assignment_algorithm task_assignment_algorithm;
	  std::shared_ptr<task_assignment_router_base> ta_router;
      world_sim_packet sim_packet;
  };
}

#