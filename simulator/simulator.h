#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "communication/agent_communicator_abstract.h"
#include "yaml_parser.h"
#include "dynamic.h"
#include <vector>
#include <map>


class simulator
{
public:
	~simulator();
	void start_sim();
	void create_communicator(int communicator_type);
	void initialize_agents(std::vector<Parsed_Agent> const&);
	void initialize (Parsed_World const&);
	void update_bonus_variables();

private:
	std::vector<dynamic*> dynamic_module;
	agent_communicator_abstract* communicator;
	world_sim_packet sim_packet;
	std::vector<index_map> agent_states_to_index;
	std::vector<index_map> agent_commands_to_index;
	index_map agents_name_to_index;
	
	
	std::map<std::string,control_command_packet> commands;
	void main_loop();
	std::vector<std::string> identifiers;
	simulation_time time;
	int num_agents;
	
	
	std::vector<exprtk::expression<double> > bonus_expressions;
	exprtk::symbol_table<double> bonus_symbol_table;
	std::map<std::string,int> map_bonus_variables;
	
};


#endif // SIMULATOR_H
