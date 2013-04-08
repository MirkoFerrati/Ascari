//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

 
void task_assignment ::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
	symbol_table.add_variable("X_TASK",my_task_x,false);
	symbol_table.add_variable("Y_TASK",my_task_y,false);
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
	bool a;
	
	a = (tasklist.at(my_task).task_execution_time < (time-initial_time));
	
	return (a && (events.at(events_to_index.at("REACHED"))==Events::_TRUE));
}


// void task_assignment ::copy_solution_to_TA_vector(std::vector<double>& solution)
// {
// 	for(unsigned int j=0;j<num_task;j++)
// 	{
// 	    agent_task_assignment_vector->at(tasks_id.at(j))=solution.at(j);
// 	}
// }
 
 
 void task_assignment ::copy_cost_vector_to_C()
{
	for (unsigned int j=0;j<num_task;j++)
	{
	      C.at(j)=agent_task_cost_vector->at(tasks_id.at(j));
	}
}


task_assignment ::~task_assignment()
{

}

