#ifndef FORMATION_CONTROL_SIMULATOR_H
#define FORMATION_CONTROL_SIMULATOR_H
#ifdef ISSIMULATOR
#include <../plugins/abstract_simulator_plugin.h>

#include "formation_control_types.h"
#include "formation_control_router.hpp"

#include <yaml_parser.h>
#include <types/world_sim_packet.h>
#include "../simulator/simulator.h"

class formation_control_simulator : public abstract_simulator_plugin
{
public:
  formation_control_simulator(simulator* s);

  bool initialize(Parsed_World const& w);
  void run_plugin();
  void stop();
  
private:
  int num_agents;
  world_sim_packet sim_packet;
  formation_control_router* router;
};

#endif //ISSIMULATOR
#endif // FORMATION_CONTROL_SIMULATOR_H
