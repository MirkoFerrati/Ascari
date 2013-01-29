//written by Alessandro Settimi
#ifndef TASK_ASSIGNMENT_H
#define TASK_ASSIGNMENT_H
#include <vector>

#include "plugin_module.h"
#include <yaml_parser.h>
#include <typedefs.h>
#include <boost/thread.hpp>
#include <task_assignment_communicator.h>
#include <bilp_problem.h>

class task_assignment: public Plugin_module
{
	
public:
	task_assignment(const Parsed_World& world, const Parsed_Agent& agent, std::map< transition, Events >& events, const std::map<std::string,transition>& events_to_index);
	void createAgentIdAndTaskIdVectorFromParsedWorld(const Parsed_World& wo);
	void createTaskListFromParsedWorld(const Parsed_World& wo);
	void createTaskCostMatrixFromParsedWorld(const Parsed_Agent& a);
	void inizializeTaskAssignmentMatrix();
	void run_plugin();
	void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
	void compileExpressions(exprtk::symbol_table< double >& symbol_table);
	void printTaskAssignmentMatrix();
	void printTaskCostMatrix();

	~task_assignment();

	
private:
	task_assignment_namespace::agent_id my_id;
	
	task_assignment_namespace::task_list tasklist;
	task_assignment_namespace::task current_task;
	
	std::vector<task_assignment_namespace::agent_id> agents_id;
	
	std::vector<task_assignment_namespace::task_id> tasks_id;
	
	task_assignment_namespace::task_cost_matrix task_cost_matrix;
	
	task_assignment_namespace::task_assignment_matrix task_assignment_matrix;
	
	task_assignment_namespace::task_cost_vector* agent_task_cost_vector;
	
	task_assignment_namespace::task_assignment_vector* agent_task_assignment_vector;
	
	bool task_assigned;
	bool stop;
	
	double speed;
	exprtk::expression<double> distance_to_target;
	std::map< transition, Events >& events;
	const std::map<std::string,transition>& events_to_index;
	
	task_assignment_communicator_base* ta_communicator;
	
	task_assignment_namespace::task_assignment_algorithm task_assignment_algorithm;
	
	int subgradient_algorithm();
	
	int solution_exchange_algorithm();
	
	void setTaskStop(bool stop);
	bool task_made();
	
	std::shared_ptr<std::mutex> ptr_receive_mutex;
	//std::shared_ptr<std::mutex> ptr_send_mutex;
	bool converge;
	bool fresh_data;
	bool not_started;
	
	//TODO: mettere tutti i dati possibili come shared_ptr, passarli al costruttore come riferimento e tanti saluti, quello che modifichi nel main thread lo modifichi anche nel communicator
	//al costruttore passi l'oggetto puntato 
	
	//task_assignment_namespace::task_assignment_packet_base* data_send;
	//std::vector<task_assignment_namespace::task_assignment_packet_base>* data_receive;
	
	//std::unique_ptr<task_assignment_namespace::task_assignment_packet_base> data_send;
	//std::unique_ptr<std::vector<task_assignment_namespace::task_assignment_packet_base>> data_receive;
	
	bilp_problem ta_problem;
	void initialize_bilp_problem(std::map<task_assignment_namespace::agent_id,task_assignment_namespace::task_cost_vector>& m);
	std::vector<double> C;
	void copy_solution_to_TA_matrix(std::vector<bool> solution);
};

#endif // TASK_ASSIGNMENT_H
