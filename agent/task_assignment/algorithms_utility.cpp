//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

void task_assignment::convergence_control_routine(unsigned int w)
{
     if (w==agents_id.size()-1)
     {
       	    std::cout<<std::endl<<"---------------- STOP ----------------"<<std::endl<<std::endl;
	    std::cout<<"Converge, ";
	    printTaskAssignmentMatrix();
	    printTaskCostMatrix();
	    converge=true;
     }
    
     if(!converge)
     {
	    printTaskAssignmentMatrix();
	    printTaskCostMatrix();
     }
}

void task_assignment::resolve_bilp_problem()
{
    std::vector<bool> solution;
      
    ta_problem.solve(solution);

    copy_solution_to_TA_matrix(solution);
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