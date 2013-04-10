//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;


// void task_assignment::resolve_assignment_problem()
// {
//     std::vector<double> solution;
//       
//     ta_problem.solve(solution);
// 
//     copy_solution_to_TA_vector(solution);
//
//     solution.clear();
// }


double task_assignment :: euclidean_distance_from_task(task_assignment_namespace::task_id task_id)
{
	return sqrt((x.value()-tasklist.at(task_id).task_position[0])*(x.value()-tasklist.at(task_id).task_position[0])+(y.value()-tasklist.at(task_id).task_position[1])*(y.value()-tasklist.at(task_id).task_position[1]));
}


void task_assignment::update_distance_vector()
{
    for (unsigned int i=0;i<tasks_id.size();i++)
    {
	      D.at(i) = euclidean_distance_from_task(tasks_id.at(i));
    }
}


void task_assignment ::update_remaining_times()
{
      for(unsigned int i=0;i<num_task;i++)
      {
		if(tasklist.at(tasks_id.at(i)).task_deadline!=0)remaining_times_to_deadline.at(tasks_id.at(i))=time_to_deadline(tasks_id.at(i));
      }
}

double task_assignment ::time_to_deadline(task_assignment_namespace::task_id task_id)
{
	double temp = tasklist.at(task_id).task_deadline-time;
	
	if (temp > 0) return temp;
	  
	if (temp < 0) 
	{
	    if(!done_task.at(task_id) && !tasklist.at(task_id).executing) std::cout<<"Deadline per il task "<<task_id<<" superata!";
	    return 0;
	}
	
	return 0;
}


void task_assignment::update_costs_with_deadlines()
{
    for (unsigned int i=0;i<tasks_id.size();i++)
    {
	  agent_task_cost_vector->at(tasks_id.at(i)) /= time_to_deadline(tasks_id.at(i));
    }
}


bool task_assignment::check_expiring_task_selection(task_assignment_namespace::task_id task_id)
{
      double min=INF;
      
      for (unsigned int j=0;j<agents_id.size();j++)
      {
	    if (min > task_cost_matrix.at(agents_id.at(j)).at(task_id))
		    min = task_cost_matrix.at(agents_id.at(j)).at(task_id);
      }
      
      if (min==agent_task_cost_vector->at(task_id)) return true;
      return false;
}

void task_assignment::update_costs_with_expiring_deadlines(task_cost_vector& basic_values)
{//per ora è semplicemente se c'è una deadline, ancora è statico
    
      for (unsigned int i=0;i<tasks_id.size();i++)
      {
	    if (tasklist.at(tasks_id.at(i)).task_deadline!=0 && agent_task_cost_vector->at(tasks_id.at(i)) != INF)
	    {
		  for (unsigned int j=0;j<agents_id.size();j++)
		  {
			if (check_expiring_task_selection(tasks_id.at(i)))
			{
			      for (unsigned int k=0;k<tasks_id.size();k++)
			      {
				    if (tasks_id.at(k) != tasks_id.at(i))
				    {
					  agent_task_cost_vector->at(tasks_id.at(k))=INF;
				    }
			      }
			}
			else
			{
			      for (unsigned int k=0;k<tasks_id.size();k++)
			      {
				    if (tasks_id.at(k) != tasks_id.at(i))
				    {
					  agent_task_cost_vector->at(tasks_id.at(k))=basic_values.at(tasks_id.at(k));
				    }
			      }
			}
		  }
	    }
      }
}