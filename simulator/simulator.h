#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "communication/agent_communicator_abstract.h"
#include "yaml_parser.h"
#include "Dynamic_module.h"
#include <vector>
#include <map>


class simulator
{
public:
	simulator(int argc, char **argv);
	~simulator();
	void start_sim();
	void create_communicator(int communicator_type);
	
private:
	//Dynamic_module dynamic_module;
	agent_communicator_abstract* communicator;
	agents_name_to_states states_index;
	
	std::vector<control_command_packet> commands;
	index_map agent_name_to_index; 
	void main_loop();
	std::vector<std::string> identifiers;
	simulation_time time;
};

#endif // SIMULATOR_H
