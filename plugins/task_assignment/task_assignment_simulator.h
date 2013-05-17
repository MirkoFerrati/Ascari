#ifndef TASK_ASSIGNMENT_SIMULATOR_H
#define TASK_ASSIGNMENT_SIMULATOR_H
#ifdef ISSIMULATOR
#include <../plugins/abstract_simulator_plugin.h>
#include "task_assignment_router.hpp"
#include "task_assignment_types.h"
#include <yaml_parser.h>
#include <types/world_sim_packet.h>
#include "../simulator/simulator.h"


class task_assignment_simulator : public abstract_simulator_plugin
{
public:
  task_assignment_simulator( simulator* s );

	bool initialize(Parsed_World const& w);
	void run_plugin();
     void stop();
private:
  int num_agents;
    	bool ta_router_started;
	
	task_assignment_namespace::task_assignment_algorithm task_assignment_algorithm;
	std::shared_ptr<task_assignment_router_base> ta_router;
    world_sim_packet sim_packet;
};

#endif //ISSIMULATOR
#endif // TASK_ASSIGNMENT_SIMULATOR_H
