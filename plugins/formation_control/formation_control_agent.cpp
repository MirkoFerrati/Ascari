#include "../agent/agent.h"

#include "formation_control_plugin.h"
#include "formation_control_agent.h"

#include <iostream>
#include <cmath>

formation_control_agent::formation_control_agent(agent* a, Parsed_World* parsed_world)
: my_id(a->identifier), agent_to_simulator_communicator(NULL)
{
  Parsed_Agent parsed_agent = parsed_world->agents.front();
  this->my_leader = (reinterpret_cast<formation_control_parsed_agent*>(parsed_agent.parsed_items_from_plugins[0]))->leader;
  this->wingmen = (reinterpret_cast<formation_control_parsed_agent*>(parsed_agent.parsed_items_from_plugins[0]))->wingmen;

  this->motion_planning_state = STATE_INITIAL;
  
//   DA SPOSTARE NEL FILE YAML
  this->theta_final = (3.14151)/4;
  this->phi_final = 0;
  this->R   = 1;
  this->D   = 5;
  this->v   = 1;
  this->w_min = (-1/this->R) + (4/this->D) + 0.05;		// -0.15
  this->w_max = (1/this->R) - (4/this->D) - 0.05; 		//  0.15

}

formation_control_agent::~formation_control_agent(){}

void formation_control_agent::dubins_trajectory()
{
  /* VARIABILI DA SPOSTARE NELLA CLASSE E NEL FILE YAML */
/*  double k1 = 0.5;
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
*/   
}

void formation_control_agent::wingman_code()
{
  double leader_gamma = this->phi_final - this->formation_packet.agent_state.state[STATE_THETA];
  double my_gamma = this->phi_final - this->theta.value();
   
  if( (my_gamma-leader_gamma > -0.01) && (my_gamma-leader_gamma < 0.01) )
  {
    if (my_gamma*leader_gamma >= 0)
    {
       /* Case A: gamma1 == gamma2 */
      this->w = this->formation_packet.omega;
    }
    else
    {
       /* Case B: gamma1 == -gamma2 */
      this->w = (4*sin(leader_gamma))/this->D - this->formation_packet.omega;
    }
  }
  else 
  {
    /* Case C: gamma1 != +/- gamma2 */
    double k_w = 0.3;
    this->w = this->formation_packet.omega + k_w*(my_gamma-leader_gamma);
  }
  
}

void formation_control_agent::update_leader_state()
{
  double tol = 0.12; //0.03; // REGOLARE IN FUNZIONE DI W_MAX
  double delta_theta_max = -0.5;
std::cout << "STATE: " << this->motion_planning_state << std::endl;
  switch(this->motion_planning_state)
  {
    case STATE_INITIAL:
	this->initial_gamma_correction = this->gamma;
	this->motion_planning_state = STATE_GAMMA_CORRECTION;
      break;
    case STATE_GAMMA_CORRECTION:    
std::cout << "GAMMA: " << this->gamma << std::endl;      
      if(fabs(this->gamma) < tol){
	this->delta_theta = this->phi_final - this->theta.value();	// phi = theta because gamma = 0
//	if (fabs(this->delta_theta - delta_theta_max) > 0)
//	  this->delta_theta = delta_theta_max;	
	this->gamma_tilde = acos((this->D*this->w_symmetric_max*this->delta_theta)/4 + 1);
      	this->motion_planning_state = STATE_PHI_CORRECTION_1;	
      }      
      break;
    case STATE_PHI_CORRECTION_1: 
std::cout << "GAMMA_TILDE: " << this->gamma_tilde << " GAMMA: " << this->gamma << std::endl;
std::cout << "W_SYMM: " << this->w_symmetric_max << " D_TH: " << this->delta_theta <<std::endl; 
std::cout << "PHI: " << this->phi << std::endl;  
      if(fabs(this->gamma_tilde - this->gamma) < tol)
	this->motion_planning_state = STATE_PHI_CORRECTION_2;
      break;
    case STATE_PHI_CORRECTION_2:      
std::cout << "PHI: " << this->phi << " GAMMA: " << this->gamma << std::endl;  
      if( fabs(this->gamma) < tol ){			// SE GAMMA=0, ALLORA GUARDO PHI
	if(fabs(this->phi_final - this->phi) < tol)
	  this->motion_planning_state = STATE_THETA_CORRECTION;
	else
	{
	  this->delta_theta = this->phi_final - this->theta.value();
//	  if (fabs(this->delta_theta - delta_theta_max) > 0)
//	    this->delta_theta = delta_theta_max;
	  this->gamma_tilde = acos((this->D*this->w_symmetric_max*this->delta_theta)/4 + 1);
	  this->motion_planning_state = STATE_PHI_CORRECTION_1;
	}
      }
      break;
    case STATE_THETA_CORRECTION:
      if(fabs(this->theta_final - this->theta.value()) < tol)
	this->motion_planning_state = STATE_FINAL;
      break;
    case STATE_FINAL:
      std::cout << "GAMMA FINAL: " << this->gamma << std::endl;  
      std::cout << "PHI FINAL: " << this->phi << std::endl;  
      std::cout << "THETA FINAL: " << this->theta.value() << std::endl;  
      break;
  }
}

void formation_control_agent::leader_code()
{
  
  switch(this->motion_planning_state)
  {
    case STATE_INITIAL:
      this->w = 0;
      break;
      
    case STATE_GAMMA_CORRECTION:
      if (this->initial_gamma_correction > 0)	// SE VADO PIANO OK, ALTRIMENTI SE VADO OLTRE, W NON CAMBIA VERSO
	this->w = -(1/this->R)+(2/this->D)*(sin(this->initial_gamma_correction)-sin(this->gamma));
      else
	this->w = (1/this->R)-(2/this->D)*(sin(this->initial_gamma_correction)-sin(this->gamma));
      break;
      
    case STATE_PHI_CORRECTION_1:    
      if (this->delta_theta > 0)				
	this->w = this->w_min + (2/this->D)*fabs(sin(this->gamma)) + (2/this->D)*sin(this->gamma);
      else
	this->w = this->w_max -(2/this->D)*fabs(sin(this->gamma)) + (2/this->D)*sin(this->gamma);
      break;
      
    case STATE_PHI_CORRECTION_2:      
      if (this->delta_theta > 0)		
	this->w = this->w_max -(2/this->D)*fabs(sin(this->gamma)) + (2/this->D)*sin(this->gamma);
      else
	this->w = this->w_min + (2/this->D)*fabs(sin(this->gamma)) + (2/this->D)*sin(this->gamma);
      break;
      
    case STATE_THETA_CORRECTION:
      if (this->theta_final - this->theta.value() > 0)
	this->w = this->w_max;
      else
	this->w = this->w_min;
      break;
      
    case STATE_FINAL:
	this->w = 0;
//      this->dubins_trajectory();
      break;
  }
  
  // SE HO FATTO GIA' 1 GIRO, DIMINUISCO W  (COMUNQUE THETA CONTINUA A SALIRE)
/*  static double angle = 0;
  static bool decrease = 0;
  int old_motion_planning_state = this->motion_planning_state;
  if (this->motion_planning_state == old_motion_planning_state){
    if (angle - this->theta.value() > 6.283 || angle - this->theta.value() < -6.283){
    std::cout << "1 GIRO ! W: " << this->w <<std::endl << std::endl,
    angle = this->theta.value();
    decrease = 1;
    }
  }
  else
    decrease = 0;
  if (decrease)    
    this->w -= 0.3;
  */
}

void formation_control_agent::update_formation_variables()
{
  this->phi = atan2(fabs(this->y.value() - formation_packet.agent_state.state[STATE_Y]), fabs(this->x.value() - formation_packet.agent_state.state[STATE_X]));
  this->gamma = this->phi - this->theta.value();
  this->w_symmetric_max = this->w_max - (2/this->D)*fabs(sin(this->gamma));
}


void formation_control_agent::send_formation_control_information()
{
  this->packet_to_send.agent_state.identifier = this->my_id;
  this->packet_to_send.agent_state.state[STATE_X] = this->x.value();
  this->packet_to_send.agent_state.state[STATE_Y] = this->y.value();
  this->packet_to_send.agent_state.state[STATE_THETA] = this->theta.value();
  this->packet_to_send.omega = this->w;
  this->agent_to_simulator_communicator->send(); 
}

void formation_control_agent::run_plugin()
{
   if(this->agent_to_simulator_communicator == NULL)
  {
    if(!this->my_id.compare(this->my_leader))
      this->agent_to_simulator_communicator = new formation_control_communicator(this->my_id, &this->packet_to_send, this->wingmen.at(0));
    else
      this->agent_to_simulator_communicator = new formation_control_communicator(this->my_id, &this->packet_to_send, this->my_leader);
    
    this->packet_to_send.agent_state.identifier = this->my_id;			// primo messaggio "vuoto" per sbloccare alla prima esecuzione la run_plugin
    this->agent_to_simulator_communicator->send();
  }
  
  this->is_leader = !this->my_id.compare(this->my_leader);
  
  this->formation_packet = this->agent_to_simulator_communicator->get_leader_data_blocking();  
  
  std::cout << "New Data!--------------------------------------"  << std::endl;
  std::cout << formation_packet.agent_state.identifier << " ";
  std::cout << formation_packet.agent_state.state[STATE_X] << " ";
  std::cout << formation_packet.agent_state.state[STATE_Y] << " ";
  std::cout << formation_packet.agent_state.state[STATE_THETA] << std::endl;
  std::cout << "-----------------------------------------------"  << std::endl;
    
  double distance = (this->x.value() - formation_packet.agent_state.state[STATE_X])*(this->x.value() -formation_packet.agent_state.state[STATE_X]);
  distance += (this->y.value() - formation_packet.agent_state.state[STATE_Y])*(this->y.value() - formation_packet.agent_state.state[STATE_Y]);
  distance = sqrt(distance);
  
  this->update_formation_variables();
  
  if(this->is_leader)
  {
    this->update_leader_state();
    this->leader_code();
  }
  else
  {
    this->wingman_code();    
  }
    
  this->send_formation_control_information();
}


void formation_control_agent::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
  symbol_table.add_variable("V_P", this->v, false);
  symbol_table.add_variable("W_P", this->w, false);
//  symbol_table.add_variable("ENABLED", this->vehicle_enabled, false);
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