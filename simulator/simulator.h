#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "communication/agent_communicator_abstract.h"
#include "yaml_parser.h"
#include "dynamic.h"
#include <vector>
#include <map>
#include <udp_agent_router.hpp>


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

private:
	int max_loops;
	
	std::vector<dynamic*> dynamic_module;
	agent_communicator_abstract* communicator;
	world_sim_packet sim_packet;
	std::vector<index_map> agent_states_to_index;
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
};


#endif // SIMULATOR_H
