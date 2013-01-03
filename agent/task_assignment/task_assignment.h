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
    void createTaskListFromParsedAgent(const Parsed_Agent& agent);
    void createTaskCostMatrixFromParsedWorld(const Parsed_World& wo);
    void run_plugin();
    void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
    void compileExpressions(exprtk::symbol_table< double >& symbol_table);

    ~task_assignment();
	
private:
	task_list tasklist;
	task current_task;
	std::vector<task_cost_vector> task_cost_matrix;
	unsigned int task_number;
	unsigned int agent_number;
	
	double speed;
	exprtk::expression<double> distance_to_target;
	std::map< transition, bool >& events;
	const std::map<std::string,transition>& events_to_index;
	
	bool task_assignment_algorithm();
	void setTaskStop(bool stop);
	bool task_made();
};

#endif // TASK_ASSIGNMENT_H
