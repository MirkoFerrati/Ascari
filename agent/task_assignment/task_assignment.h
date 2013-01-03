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
    task_assignment(const Parsed_Agent& agent, std::map< transition, bool >& events, const std::map<std::string,transition>& events_to_index);
    void createTaskListFromParsedAgent(const Parsed_Agent& agent);
    void run_plugin();
    void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
    void compileExpressions(exprtk::symbol_table< double >& symbol_table);

    ~task_assignment();
    /*! 
     * Stampa su out la lista dei target provvisori e infine il target definitivo
     */
    std::ostream& toFile(std::ostream& out) const;
	
private:
	task_list tasklist;
	task current_task;
	
	double speed;
	exprtk::expression<double> distance_to_target;
	std::map< transition, bool >& events;
	const std::map<std::string,transition>& events_to_index;
	
	bool task_assignment_algorithm();
	void setTaskStop(bool stop);
	bool task_made();
};

#endif // TASK_ASSIGNMENT_H
