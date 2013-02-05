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


void task_assignment ::copy_solution_to_TA_matrix(std::vector<bool> solution)
{
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    task_assignment_matrix.at(agents_id[i]).at(tasks_id[j])=solution[i*tasks_id.size()+j];
	}
	
    }
}
 
 
 void task_assignment ::copy_cost_matrix_to_cost_vector()
{
  	C.clear();
	
	int z=0;
	
	for (unsigned int i=0;i<agents_id.size();i++)
	{
	    for (unsigned int j=0;j<tasks_id.size();j++)
	    {
		 C.push_back(task_cost_matrix.at(agents_id[i]).at(tasks_id[j]));
		 z++;
	    }
	}
}


task_assignment ::~task_assignment()
{

}

