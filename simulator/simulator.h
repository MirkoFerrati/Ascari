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
	void initialize(std::vector<Parsed_Agent> const&);

private:
	std::vector<dynamic*> dynamic_module;
	agent_communicator_abstract* communicator;
	agents_name_to_states states_index;
	std::vector<index_map> agent_states_to_index;
	std::vector<index_map> agent_commands_to_index;
	index_map agents_name_to_index;
	
	
	std::vector<control_command_packet> commands;
	void main_loop();
	std::vector<std::string> identifiers;
	simulation_time time;
};


#endif // SIMULATOR_H
