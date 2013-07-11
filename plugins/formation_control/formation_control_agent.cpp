#include "../agent/agent.h"

#include "formation_control_plugin.h"
#include "formation_control_agent.h"

#include <iostream>

formation_control_agent::formation_control_agent(agent* a, Parsed_World* parsed_world)
: my_id(a->identifier)
{
  this->x_initial = parsed_world->agents.front().initial_states.at("X");
  this->y_initial = parsed_world->agents.front().initial_states.at("Y");
  this->theta_initial = parsed_world->agents.front().initial_states.at("THETA");
  
  Parsed_Agent parsed_agent = parsed_world->agents.front();
  this->final_state = (reinterpret_cast<formation_control_parsed_agent*>(parsed_agent.parsed_items_from_plugins[0]))->final_state;
}


formation_control_agent::~formation_control_agent()
{

}


void formation_control_agent::run_plugin()
{
  double k1 = 0.1;
  double k2 = 0.1;
  double lambda2 = 1;
  
  double rho = sqrt((this->x.value())*(this->x.value()) + (this->y.value())*(this->y.value()));
  double phi = atan2(this->y.value(), this->x.value()) + 3.1415;
  double alpha = phi - this->theta.value();

  if(abs(this->x.value() - this->final_state[STATE_X]) < 8 && abs(this->y.value() - this->final_state[STATE_Y]) < 8)
  {
    this->vehicle_enabled = -10;
  }
  else
  {
    this->vehicle_enabled = 10; 
//      v = k1*rho*cos(alpha);
    v = 5; 
    if(alpha <= 0.01)
      w = k2*cos(alpha)*(alpha + lambda2*phi) + k2*alpha;
    else
      w = k2*(1/alpha)*sin(alpha)*cos(alpha)*(alpha + lambda2*phi) + k2*alpha;
   }
   
   std::cout << "X: " << x.value() << " Y: " << y.value() << std::endl;
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