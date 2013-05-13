#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "communication/agent_communicator_abstract.h"
#include "communication/viewer_communicator_abstract.h"
#include "yaml_parser.h"
#include "dynamic.h"
#include <vector>
#include <map>
#include "collisioncheckerabstract.h"
#include "random.hpp"
#include <objects/abstract_object.h>
#include <types/graph_informations.h>
#include <mutex>
#include <condition_variable>
#include <thread>
//written by Alessandro Settimi
#include "visibility/visibility.h"
#include <map2d.h>
//written by Alessandro Settimi

#include "../plugins/abstract_simulator_plugin.h"

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
	
	simulator(const simulator&)=delete;
	
private:
	int max_loops;
	int secSleep;
	bool checkCollision;
	CollisionCheckerAbstract *collisionChecker;
	std::vector<dynamic*> dynamic_module;
	simulator_namespace::agent_communicator_abstract* communicator;
	simulator_namespace::viewer_communicator_abstract* viewer_communicator;
	world_sim_packet sim_packet;
	agent_sim_packet agent_packet;
	index_map agent_states_to_index;
	std::vector<index_map> agent_commands_to_index;
	index_map agents_name_to_index;
	std::map<int,double> bonusVariables;
	std::map<std::string,int> map_bonus_variables_to_id;
	std::map<std::string,control_command_packet> commands;
	void main_loop();
	void input_loop( std::mutex& input_mutex, std::condition_variable& input_cond, volatile bool& paused, volatile bool& exit, volatile int& secSleep );
	std::vector<std::string> identifiers;
	simulation_time time;
	unsigned int num_agents;
	rndom<double> *f_rndom;
	double pi;
	std::vector<exprtk::expression<double> > bonus_expressions;
	exprtk::symbol_table<double> bonus_symbol_table;
	std::map<std::string,int> map_bonus_variables;
	//Udp_agent_router<topology_packet> topology_router;
	//Udp_agent_router<graph_packet> graph_router;
	void createObjects(const Parsed_World& world);
    void initialize_plugins ( const Parsed_World& world );
	
	
	std::vector<abstract_simulator_plugin*> plugins;
	
	
	//written by Alessandro Settimi

	//bool ta_router_started;
	//task_assignment_namespace::task_assignment_algorithm task_assignment_algorithm;
	//task_assignment_router_base* ta_router;

	//written by Alessandro Settimi
	
	std::map<int,std::shared_ptr<visibleArea> > agents_visibility;
	visibleArea* world_map;
};


#endif // SIMULATOR_H
