//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>

#include "../HACK_KDEVELOP.h"

using namespace task_assignment_namespace;

double task_assignment::norm2(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

double task_assignment :: euclidean_distance_from_task(task_assignment_namespace::task_id task_id)
{
	return norm2(x.value(),y.value(),reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(task_id)->getState())->task_position[0],reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(task_id)->getState())->task_position[1]);
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
		if(reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i))->getState())->task_deadline!=0)remaining_times_to_deadline.at(tasks_id.at(i))=time_to_deadline(tasks_id.at(i));
      }
}

double task_assignment ::time_to_deadline(task_assignment_namespace::task_id task_id)
{
	double temp = reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(task_id)->getState())->task_deadline-time;
	
	if (temp > 0) return temp;
	  
	if (temp < 0) 
	{
	    if( reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(task_id)->getState())->available && !reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(task_id)->getState())->executing) std::cout<<"Deadline per il task "<<task_id<<" superata!";
	    return 0;
	}
	
	return 0;
}