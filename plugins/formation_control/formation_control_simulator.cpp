#include "formation_control_simulator.h"
#include "formation_control_parser.h"

#include <types/world_sim_packet.h>
#include <boost/concept_check.hpp>

formation_control_simulator::formation_control_simulator(simulator* s)
  : sim_packet(s->sim_packet)
{
   this->router = NULL;
}


bool formation_control_simulator::initialize(const Parsed_World& w)
{
  this->num_agents = w.agents.size();  
  return true;
}


void formation_control_simulator::stop(){}

std::list<abstract_object*>* formation_control_simulator::create_objects()
{
  std::list<abstract_object*>* obj = new std::list<abstract_object*>;
  return obj;
  
}
  
  
void formation_control_simulator::run_plugin()
{
  if(this->router == NULL)
    this->router = new formation_control_router(this->num_agents);
  
  this->router->sync_messages();
  
}
