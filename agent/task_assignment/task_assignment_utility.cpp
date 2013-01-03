//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace std;

void task_assignment::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
	symbol_table.add_variable("X_TASK",current_task.task_position[0],false);
	symbol_table.add_variable("Y_TASK",current_task.task_position[0],false);
	symbol_table.add_variable("V_T",speed,false);
}


void task_assignment::compileExpressions(exprtk::symbol_table< double >& symbol_table)
{
	exprtk::parser<double> parser;
	
	distance_to_target.register_symbol_table(symbol_table);
	if (!parser.compile("sqrt((X_TASK-X)^2+(Y_TASK-Y)^2)",distance_to_target))
	{
		ERR("impossibile creare l'espressione: %s","sqrt(sqr(X_TASK)+sqr(Y_TASK))");
		throw "impossibile creare l'espressione";
	}
	
}


void task_assignment::setTaskStop(bool stop)
{
	events.at(events_to_index.at("STOPPED"))=stop;
	events.at(events_to_index.at("STARTED"))=!stop;
}


bool task_assignment::task_made()
{
	return events.at(events_to_index.at("REACHED"));
}


task_assignment::~task_assignment()
{
}

