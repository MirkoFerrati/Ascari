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
#include <random.hpp>

class task_assignment: public Plugin_module
{
	
public:
	task_assignment(const Parsed_World& world, const Parsed_Agent& agent,simulation_time& time, std::map< transition, Events >& events, const std::map<std::string,transition>& events_to_index);
	void createAgentIdAndTaskIdVectorFromParsedWorld(const Parsed_World& wo);
	void createTaskListFromParsedWorld(const Parsed_World& wo);
	void createTaskCostMatrixFromParsedWorld(const Parsed_Agent& a);
	void inizializeTaskAssignmentMatrix();
	void run_plugin();
	void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
	void compileExpressions(exprtk::symbol_table< double >& symbol_table);
	void printTaskAssignmentMatrix();
	void printTaskCostMatrix(task_assignment_namespace::task_cost_matrix& C_matrix);

	~task_assignment();

	
private:
	rndom<double> rndom_double;
	
	simulation_time& time;
    
	task_assignment_namespace::agent_id my_id;
	double x0,y0;
	
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
	
	void convergence_control_routine(unsigned int w);
	
	void resolve_bilp_problem();
	
	void update_costs_with_position();
	
	double distance_from_task(task_assignment_namespace::task_id task_id);
	
	void update_costs_with_deadlines();
	
	double time_to_deadline(task_assignment_namespace::task_id task_id);
	
	void update_costs_with_expiring_deadlines(task_assignment_namespace::task_cost_vector& basic_values);
	
	bool check_expiring_task_selection(task_assignment_namespace::task_id task_id);
	
	//algorithms
	task_assignment_namespace::task_id subgradient_algorithm();
	
	task_assignment_namespace::task_id solution_exchange_algorithm();
	
	task_assignment_namespace::task_id cost_exchange_algorithm();
	//
	
	void setTaskStop(bool stop);
	bool task_made();
	
	std::shared_ptr<std::mutex> ptr_receive_mutex;
	
	bool converge;
	bool fresh_data;
	bool not_started;
	
	//algorithms packets ptr
	std::shared_ptr<task_assignment_namespace::cost_exchange_packet> ptr_cost_exchange_packet;
	
	std::shared_ptr<task_assignment_namespace::solution_exchange_packet> ptr_solution_exchange_packet;
	
	std::shared_ptr<task_assignment_namespace::subgradient_packet> ptr_subgradient_packet;
	//
	
	bilp_problem ta_problem;
	void initialize_bilp_problem();
	std::vector<double> C;
	void copy_solution_to_TA_matrix(std::vector<bool> solution);
	void copy_cost_matrix_to_cost_vector(task_assignment_namespace::task_cost_matrix& C_matrix);

};

#endif // TASK_ASSIGNMENT_H
