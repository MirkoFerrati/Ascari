//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

 
void task_assignment ::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
	symbol_table.add_variable("X_TASK",current_task.task_position[0],false);
	symbol_table.add_variable("Y_TASK",current_task.task_position[1],false);
	symbol_table.add_variable("V_T",speed,false);
}

 
void task_assignment ::compileExpressions(exprtk::symbol_table< double >& symbol_table)
{
	exprtk::parser<double> parser;
	
	distance_to_target.register_symbol_table(symbol_table);
	if (!parser.compile("sqrt((X_TASK-X)^2+(Y_TASK-Y)^2)",distance_to_target))
	{
		ERR("impossibile creare l'espressione: %s","sqrt(sqr(X_TASK)+sqr(Y_TASK))");
		throw "impossibile creare l'espressione";
	}
	
}

 
void task_assignment ::setTaskStop(bool stop)
{
	events.at(events_to_index.at("STOPPED"))=stop?Events::_TRUE:Events::_FALSE;
	events.at(events_to_index.at("STARTED"))=!stop?Events::_TRUE:Events::_FALSE;
}

 
bool task_assignment ::task_made()
{
	return events.at(events_to_index.at("REACHED"))==Events::_TRUE;
}

double task_assignment ::distance_from_task(task_assignment_namespace::task_id task_id)
{
	return sqrt((x0-tasklist.at(task_id).task_position[0])*(x0-tasklist.at(task_id).task_position[0])+(y0-tasklist.at(task_id).task_position[1])*(y0-tasklist.at(task_id).task_position[1]));
}

double task_assignment ::time_to_deadline(task_assignment_namespace::task_id task_id)
{
	double temp = tasklist.at(task_id).task_deadline-time;
	
	if (temp==-time) return 1;
	
	if (temp > 0) return temp;
	  
	if (temp < 0) 
	{
	    std::cout<<"Deadline per il task "<<task_id<<" superata";
	    ERR("!");
	    return 1;
	}
	
	return 1;
}
 
task_assignment ::~task_assignment()
{

}

