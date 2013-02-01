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


double task_assignment ::distance_from_task(task_assignment_namespace::task_id task_id)
{
	return sqrt((x0-tasklist.at(task_id).task_position[0])*(x0-tasklist.at(task_id).task_position[0])+(y0-tasklist.at(task_id).task_position[1])*(y0-tasklist.at(task_id).task_position[1]));
}


void task_assignment::update_costs_with_position()
{
    for (unsigned int i=0;i<tasks_id.size();i++)
    {
	  if(task_cost_matrix.at(my_id).at(tasks_id.at(i))!=INF)
	      task_cost_matrix.at(my_id).at(tasks_id.at(i)) += distance_from_task(tasks_id.at(i));
    }
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


void task_assignment::update_costs_with_deadlines()
{
    for (unsigned int i=0;i<tasks_id.size();i++)
    {
	  if(task_cost_matrix.at(my_id).at(tasks_id.at(i))!=INF)
	      task_cost_matrix.at(my_id).at(tasks_id.at(i)) /= time_to_deadline(tasks_id.at(i));
    }
}


void task_assignment::update_costs_with_expiring_deadlines()
{//per ora è semplicemente se c'è una deadline, ancora è statico
    task_cost_vector::iterator iter = std::min_element(task_cost_matrix.at(my_id).begin(), task_cost_matrix.at(my_id).end());
    double min_cost = iter->second;

    for (unsigned int i=0;i<agents_id.size();i++)
    {
	  if (agents_id.at(i) != my_id)
	  {
		for (unsigned int j=0;j<tasks_id.size();j++)
		{
		  
		      if (task_cost_matrix.at(my_id).at(tasks_id.at(j))!=INF)
		      {
			      double app = task_cost_matrix.at(agents_id.at(i)).at(tasks_id.at(j));
			      double deadline = tasklist.at(tasks_id.at(j)).task_deadline;
			      
			      if (tasklist.at(tasks_id.at(j)).task_deadline!=0)
			      {
				  if(app !=0 && app < task_cost_matrix.at(my_id).at(tasks_id.at(j)))
				  {
					  for (unsigned int k=0;k<tasks_id.size();k++)
					  {
						  if (task_cost_matrix.at(my_id).at(tasks_id.at(k))<INF && tasks_id.at(k)!=tasks_id.at(j) && (tasklist.at(tasks_id.at(k)).task_deadline*((min_cost-1)/(min_cost)) > deadline))// || tasklist.at(tasks_id.at(k)).task_deadline==0))
						  {
							  task_cost_matrix.at(my_id).at(tasks_id.at(k)) = (1-(deadline/tasklist.at(tasks_id.at(k)).task_deadline))*min_cost-rndom_double(0.0,1.0);
						  }
					  }
				  }
			      }
		      }
		}
	  }
    }
}


task_assignment ::~task_assignment()
{

}

