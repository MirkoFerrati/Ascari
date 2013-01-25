#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "communication/agent_communicator_abstract.h"
#include "yaml_parser.h"
#include "dynamic.h"
#include <vector>
#include <map>
#include <udp_agent_router.hpp>
#include "collisioncheckerabstract.h"
#include "random.hpp"
#include <mutex>
#include <thread>
//written by Alessandro Settimi
#include "communication/task_assignment_router.hpp"
//written by Alessandro Settimi

class simulator
{
public:
	simulator();
	~simulator();
	void start_sim(int max_loops=10000);
	void create_communicator(int communicator_type);
	void initialize_agents(std::vector<Parsed_Agent> const&);
	void initialize (Parsed_World const&);
	void update_bonus_variables();
	void setSleep(unsigned secSleep);
    void setCheckCollision(bool checkCollision);
    
	//written by Alessandro Settimi
	void create_ta_communicator();
	//written by Alessandro Settimi
    
private:
	int max_loops;
	int secSleep;
	bool checkCollision;
	CollisionCheckerAbstract *collisionChecker;
	std::vector<dynamic*> dynamic_module;
	agent_communicator_abstract* communicator;
	world_sim_packet sim_packet;
	index_map agent_states_to_index;
	std::vector<index_map> agent_commands_to_index;
	index_map agents_name_to_index;
	std::map<int,double> bonusVariables;
	std::map<std::string,int> map_bonus_variables_to_id;
	std::map<std::string,control_command_packet> commands;
	void main_loop();
	std::vector<std::string> identifiers;
	simulation_time time;
	unsigned int num_agents;
	rndom<double> *f_rndom;
	double pi;
	std::vector<exprtk::expression<double> > bonus_expressions;
	exprtk::symbol_table<double> bonus_symbol_table;
	std::map<std::string,int> map_bonus_variables;
	Udp_agent_router<topology_packet> topology_router;
	Udp_agent_router<graph_packet> graph_router;
	
	//written by Alessandro Settimi
	//task_assignment_router ta_router;
	bool ta_router_started;
	task_assignment_namespace::task_assignment_algorithm task_assignment_algorithm;
	task_assignment_router_base* ta_router;
	//task_assignment_router<task_assignment_namespace::subgradient_packet> ta_router1;
	//task_assignment_router<task_assignment_namespace::solution_exchange_packet> ta_router2;
	//written by Alessandro Settimi
};


#endif // SIMULATOR_H
