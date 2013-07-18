#ifndef FORMATION_CONTROL_AGENT_H
#define FORMATION_CONTROL_AGENT_H

#include "../HACK_KDEVELOP.h"
#ifdef ISAGENT

#include <vector>

#include <yaml_parser.h>
 
#include <random.hpp>
#include <types/events.h>

#include "../plugins/abstract_agent_plugin.h"
#include "../agent/agent.h"

#include "formation_control_types.h"
#include "formation_control_communicator.h"


class formation_control_agent : public abstract_agent_plugin
{
  double v;
  double w;

  std::string my_id;
  
  exprtk::expression<double> x;
  exprtk::expression<double> y;
  exprtk::expression<double> theta;
  
  initial_state_value x_initial;
  initial_state_value y_initial;
  initial_state_value theta_initial;
  
  std::string my_leader;
  std::vector<std::string> wingmen;
  
  formation_control_communicator* agent_to_simulator_communicator; 
  formation_control_packet packet_to_send;
  formation_control_packet packet_received;
    
  double vehicle_enabled;
  double phi;
  
  void dubins_trajectory();
  void wingmen_trajectory();

public:
  formation_control_agent(agent* a, Parsed_World* parsed_world);
  ~formation_control_agent();
  
  void run_plugin();
  void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
  void compileExpressions(exprtk::symbol_table< double >& symbol_table);
};

#endif

#endif // FORMATION_CONTROL_AGENT_H
