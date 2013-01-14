//written by Alessandro Settimi
#ifndef TASK_ASSIGNMENT_H
#define TASK_ASSIGNMENT_H
#include <vector>

#include "plugin_module.h"
#include <yaml_parser.h>
#include <typedefs.h>
#include <boost/thread.hpp>

class task_assignment: public Plugin_module
{
	
public:
    task_assignment(const Parsed_World& world, const Parsed_Agent& agent, std::map< transition, bool >& events, const std::map<std::string,transition>& events_to_index);
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
	agent_id my_id;
	
	task_list tasklist;
	task current_task;
	
	std::vector<std::string> agents_id;
	
	std::vector<std::string> tasks_id;
	
	std::map<agent_id,task_cost_vector> task_cost_matrix;
	
	std::map<agent_id,task_assignment_vector> task_assignment_matrix;
	
	task_cost_vector* agent_task_cost_vector;
	
	task_assignment_vector* agent_task_assignment_vector;
	
	bool task_assigned;
	bool stop;
	
	double speed;
	exprtk::expression<double> distance_to_target;
	std::map< transition, bool >& events;
	const std::map<std::string,transition>& events_to_index;
	
	int ta_1(); 
	bool task_assignment_algorithm();
	void setTaskStop(bool stop);
	bool task_made();
};

#endif // TASK_ASSIGNMENT_H
