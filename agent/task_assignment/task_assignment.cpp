//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace std;

task_assignment::task_assignment(const Parsed_World& world, const Parsed_Agent& agent, std::map< transition, bool >& events, const std::map<std::string,transition>& events_to_index):events(events),events_to_index(events_to_index)
{
    createTaskListFromParsedAgent(agent);
    createTaskCostMatrixFromParsedWorld(world);
    task_assigned=false;
    stop=false;
    speed=0;
}

void task_assignment::createTaskListFromParsedAgent(const Parsed_Agent& agent)
{
    agent_id=agent.name;
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
	    
	    std::cout << std::endl << "TASK " << agent.tl.tasks[i].task_id <<':'<< std::endl;
	    std::cout << "- posizione: " << agent.tl.tasks[i].task_position[0] <<' '<< agent.tl.tasks[i].task_position[1]<<' '<< agent.tl.tasks[i].task_position[2] << std::endl;
	    std::cout << "- tipo: " << agent.tl.tasks[i].task_type << std::endl;
	    std::cout << "- execution time: " << agent.tl.tasks[i].task_execution_time << std::endl;
	    std::cout << "- deadline: " << agent.tl.tasks[i].task_deadline << std::endl << std::endl;
    }
}

void task_assignment::createTaskCostMatrixFromParsedWorld(const Parsed_World& wo)
{
    agent_number=wo.agent_number;
    task_number=wo.task_number;
  
    std::cout<<std::endl<<"TASK COST MATRIX"<<std::endl;
    for(unsigned int i=0;i<agent_number;i++)
    {
	std::vector <double> app;
	task_cost_matrix.push_back(app);
	for(unsigned int j=0;j<task_number;j++)
	{
	    task_cost_matrix[i].push_back(0);
	    task_cost_matrix[i][j]=wo.task_cost_matrix[i][j];
	    std::cout<<task_cost_matrix[i][j]<<' ';
	}
	std::cout<<std::endl;
    }    
}

bool task_assignment::task_assignment_algorithm()
{
     //si deve assegnare il current_task mediante ottimizzazione
     int a=-1;
     if (agent_id=="AGENTE1") a=0;
     if (agent_id=="AGENTE2") a=1;
     if (agent_id=="AGENTE3") a=2;
     
     if (a==-1) return false;
     
     current_task=tasklist.tasks[a];
     return true;
}

void task_assignment::run_plugin()
{
     if (stop)
     {
	setTaskStop(true);
	return;
     }
  
     if (task_assigned)
     {
	if(task_made())
	{
	    std::cout<<"TASK ESEGUITO"<<std::endl;
	    task_assigned=false;
	    speed=0;
	    stop=true;
	}
     }
     else
     {
	if(!task_assignment_algorithm())
	{
	    ERR("attenzione, task assignment non riuscito");
	}
	else
	{
	    std::cout<<"CURRENT TASK: "<<current_task.task_id<<" position: ["<<current_task.task_position[0]<<' '<<current_task.task_position[1]<<' '<<current_task.task_position[2]<<"]"<<std::endl;
	    task_assigned=true;
	    speed=1;
	}
     }
}