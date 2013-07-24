//written by Alessandro Settimi
#include "task_assignment.h"


using namespace task_assignment_namespace;

 
void task_assignment ::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
	symbol_table.add_variable("X_TASK",my_task_x,false);
	symbol_table.add_variable("Y_TASK",my_task_y,false);
	symbol_table.add_variable("V_T",speed,false);
	symbol_table.add_variable("W_T_DIFF",omega,false);
	symbol_table.add_variable("W_T_DUBINS",omega_dubins,false);
	symbol_table.add_variable("SET_CHARGE",set_charge,false);
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
	
	x.register_symbol_table(symbol_table);
	if (!parser.compile("X",x))
	{
		ERR("impossibile creare l'espressione: %s","X");
		throw "impossibile creare l'espressione";
	}
	
	y.register_symbol_table(symbol_table);
	if (!parser.compile("Y",y))
	{
		ERR("impossibile creare l'espressione: %s","Y");
		throw "impossibile creare l'espressione";
	}
	
	theta.register_symbol_table(symbol_table);
	if (!parser.compile("THETA",theta))
	{
		ERR("impossibile creare l'espressione: %s","THETA");
		throw "impossibile creare l'espressione";
	}
	
	charge_.register_symbol_table(symbol_table);
	if (!parser.compile("CHARGE",charge_))
	{
		ERR("impossibile creare l'espressione: %s","CHARGE");
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
	return (reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->done);
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

