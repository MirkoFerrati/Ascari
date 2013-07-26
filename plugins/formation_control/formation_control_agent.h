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

#define STATE_INITIAL		1
#define STATE_GAMMA_CORRECTION	2
#define STATE_PHI_CORRECTION_1	3
#define STATE_PHI_CORRECTION_2	4
#define STATE_THETA_CORRECTION	5
#define STATE_FINAL		6

class formation_control_agent : public abstract_agent_plugin
{
  std::string my_id;
  
  exprtk::expression<double> x;
  exprtk::expression<double> y;
  exprtk::expression<double> theta;
  double gamma;
  double phi;
  
  double v;
  double w;
  
  double w_min;
  double w_max;
  double w_symmetric_max;
  double theta_final;
  double phi_final;
  
  /************* AGGIUSTARE ******************/
  double initial_gamma_correction;
  double delta_theta;
  double gamma_tilde;
  
  double R;
  double D;
  
  std::string my_leader;
  std::vector<std::string> wingmen;
  
  bool is_leader;
  int  motion_planning_state;
  
  formation_control_communicator* agent_to_simulator_communicator; 
  formation_control_packet packet_to_send;
  formation_control_packet formation_packet;
  
  void dubins_trajectory();
  void leader_code();
  void wingman_code();
  void update_leader_state();
  void update_formation_variables();
  void send_formation_control_information();

public:
  formation_control_agent(agent* a, Parsed_World* parsed_world);
  ~formation_control_agent();
  
  void run_plugin();
  void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
  void compileExpressions(exprtk::symbol_table< double >& symbol_table);
};

#endif

#endif // FORMATION_CONTROL_AGENT_H
