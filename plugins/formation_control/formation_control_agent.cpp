#include "../agent/agent.h"

#include "formation_control_plugin.h"
#include "formation_control_agent.h"

#include <iostream>

formation_control_agent::formation_control_agent(agent* a, Parsed_World* parsed_world)
: my_id(a->identifier), agent_to_simulator_communicator(NULL)
{
  this->x_initial = parsed_world->agents.front().initial_states.at("X");
  this->y_initial = parsed_world->agents.front().initial_states.at("Y");
  this->theta_initial = parsed_world->agents.front().initial_states.at("THETA");
  
  Parsed_Agent parsed_agent = parsed_world->agents.front();
  this->my_leader = (reinterpret_cast<formation_control_parsed_agent*>(parsed_agent.parsed_items_from_plugins[0]))->leader;
  this->wingmen = (reinterpret_cast<formation_control_parsed_agent*>(parsed_agent.parsed_items_from_plugins[0]))->wingmen;

//   DA SPOSTARE NEL FILE YAML
  this->phi = 0;
}


formation_control_agent::~formation_control_agent(){}


void formation_control_agent::dubins_trajectory()
{
  /* VARIABILI DA SPOSTARE NELLA CLASSE E NEL FILE YAML */
  double k1 = 0.5;
  double k2 = 0.5;
  double lambda2 = 0.5;
  
  double rho = sqrt((this->x.value())*(this->x.value()) + (this->y.value())*(this->y.value()));
  double phi = atan2(this->y.value(), this->x.value()) + 3.1415;
  double alpha = phi - this->theta.value();

  if(abs(this->x.value()) < 8 && abs(this->y.value()) < 8)
  {
    this->vehicle_enabled = -10;
    std::cout << "Fermati!!!" << std::endl;
  }
  else
  {
    this->vehicle_enabled = 10; 
    
    if(alpha <= 0.01)
      this->w = k2*cos(alpha)*(alpha + lambda2*phi) + k2*alpha;
    else
      this->w = k2*(1/alpha)*sin(alpha)*cos(alpha)*(alpha + lambda2*phi) + k2*alpha;
   }
}


void formation_control_agent::run_plugin()
{
  
  if(this->agent_to_simulator_communicator == NULL)
    this->agent_to_simulator_communicator = new formation_control_communicator(this->my_id, &this->packet_to_send, this->my_leader);
  
  this->v = 1;
  
  if(this->agent_to_simulator_communicator->get_new_data(&this->packet_received))
  {
    std::cout << "New Data!--------------------------------------"  << std::endl;
    std::cout << packet_received.agent_state.identifier << " ";
    std::cout << packet_received.agent_state.state[STATE_X] << " ";
    std::cout << packet_received.agent_state.state[STATE_Y] << " ";
    std::cout << packet_received.agent_state.state[STATE_THETA] << std::endl;
    std::cout << "-----------------------------------------------"  << std::endl;
    
    double distance = (this->x.value() - packet_received.agent_state.state[STATE_X])*(this->x.value() - packet_received.agent_state.state[STATE_X]);
    distance += (this->y.value() - packet_received.agent_state.state[STATE_Y])*(this->y.value() - packet_received.agent_state.state[STATE_Y]);
    distance = sqrt(distance);
    std::cout << "Distanza agenti: " << distance << std::endl;
    
    if(!this->my_id.compare(this->my_leader))
    {
	this->dubins_trajectory();
    }
    else
    {
      double leader_gamma = this->phi - this->packet_received.agent_state.state[STATE_THETA];
      double my_gamma = this->phi - this->theta.value();
      
      if(abs(my_gamma - leader_gamma) < 0.01)
      {
	/* Case A */
	this->w = this->packet_received.omega;
      }
      else
      {
	double k = 10.0;
	this->w = this->packet_received.omega + k*(my_gamma - leader_gamma);
	/* Case B */
      }
    }
  }
  
  this->packet_to_send.agent_state.identifier = this->my_id;
  this->packet_to_send.agent_state.state[STATE_X] = this->x.value();
  this->packet_to_send.agent_state.state[STATE_Y] = this->y.value();
  this->packet_to_send.agent_state.state[STATE_THETA] = this->theta.value();
  this->packet_to_send.omega = this->w;
  this->agent_to_simulator_communicator->send();
}


void formation_control_agent::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
  symbol_table.add_variable("V_P", this->v, false);
  symbol_table.add_variable("W_P", this->w, false);
  symbol_table.add_variable("ENABLED", this->vehicle_enabled, false);
}


void formation_control_agent::compileExpressions(exprtk::symbol_table< double >& symbol_table)
{
  exprtk::parser<double> parser;

  x.register_symbol_table(symbol_table);

  if (!parser.compile("X",x))
  {
    ERR("impossibile creare l'espressione: %s","X");
    throw "impossibile creare l'espressione";
  }

  y.register_symbol_table(symbol_table);
  if (!parser.compile("Y",y))
  {
    ERR("impossibile creare l'espressione: %s","Y");
    throw "impossibile creare l'espressione";
  }

  theta.register_symbol_table(symbol_table);
  if (!parser.compile("THETA",theta))
  {
    ERR("impossibile creare l'espressione: %s","THETA");
    throw "impossibile creare l'espressione";
  }
}