//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace std;

task_assignment::task_assignment(const Parsed_Agent& agent, std::map< transition, bool >& events, const std::map<std::string,transition>& events_to_index):events(events),events_to_index(events_to_index)
{
    createTaskListFromParsedAgent(agent);
}

void task_assignment::createTaskListFromParsedAgent(const Parsed_Agent& agent)
{
    tasklist.task_number=agent.tl.task_number;
    
    for (int i=0; i< tasklist.task_number;i++)
    {	
	    task app;
	    tasklist.tasks.push_back(app);
	    
	    tasklist.tasks[i].task_id=agent.tl.tasks[i].task_id;
	    tasklist.tasks[i].task_position[0]=agent.tl.tasks[i].task_position[0];
	    tasklist.tasks[i].task_position[1]=agent.tl.tasks[i].task_position[1];
	    tasklist.tasks[i].task_position[2]=agent.tl.tasks[i].task_position[2];
	    tasklist.tasks[i].task_type=agent.tl.tasks[i].task_type;
	    tasklist.tasks[i].task_execution_time=agent.tl.tasks[i].task_execution_time;
	    tasklist.tasks[i].task_deadline=agent.tl.tasks[i].task_deadline;
	    
	    std::cout << std::endl << "TASK in AGENT: " << agent.tl.tasks[i].task_id << std::endl;
	    std::cout << "- posizione: " << agent.tl.tasks[i].task_position[0] <<' '<< agent.tl.tasks[i].task_position[1]<<' '<< agent.tl.tasks[i].task_position[2] << std::endl;
	    std::cout << "- tipo: " << agent.tl.tasks[i].task_type << std::endl;
	    std::cout << "- execution time: " << agent.tl.tasks[i].task_execution_time << std::endl;
	    std::cout << "- deadline: " << agent.tl.tasks[i].task_deadline << std::endl << std::endl;
    }
}

bool task_assignment::task_assignment_algorithm()
{
     return false;
}

void task_assignment::run_plugin()
{
     if(!task_assignment_algorithm()) ERR("attenzione, task assignment non riuscito");
}