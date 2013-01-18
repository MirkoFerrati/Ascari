//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace std;

task_assignment::task_assignment(const Parsed_World& world, const Parsed_Agent& agent, std::map< transition, bool >& events, const std::map<std::string,transition>& events_to_index):my_id(agent.name),events(events),events_to_index(events_to_index),data_mutex(),ptr_mutex(&data_mutex),ta_communicator(data_r,ptr_mutex,converge,my_id,fresh_data)
{
    createAgentIdAndTaskIdVectorFromParsedWorld(world);
    createTaskListFromParsedWorld(world);
    createTaskCostMatrixFromParsedWorld(agent);
    inizializeTaskAssignmentMatrix();
    
    task_assigned=false;
    stop=false;
    speed=0;
    
    printTaskAssignmentMatrix();
    printTaskCostMatrix();
    
    data_s.g=1;
    data_s.agent_id=my_id;
    
    converge=false;
    fresh_data=true;
    
    not_started=true;
}


void task_assignment::createAgentIdAndTaskIdVectorFromParsedWorld(const Parsed_World& wo)
{
    for (unsigned int i=0;i<wo.agents.size();i++)
	agents_id.push_back(wo.agents[i].name);  
    
    for (unsigned int i=0;i<wo.tl.size();i++)
	tasks_id.push_back(wo.tl[i].task_id);
}


void task_assignment::createTaskListFromParsedWorld(const Parsed_World& wo)
{
    
    for (unsigned int i=0; i< wo.tl.size();i++)
    {	
	    task app;
	    tasklist.push_back(app); //posso fare la push_back di tutto il task?
	    
	    tasklist[i].task_id=wo.tl[i].task_id;
	    tasklist[i].task_position[0]=wo.tl[i].task_position[0];
	    tasklist[i].task_position[1]=wo.tl[i].task_position[1];
	    tasklist[i].task_position[2]=wo.tl[i].task_position[2];
	    tasklist[i].task_type=wo.tl[i].task_type;
	    tasklist[i].task_execution_time=wo.tl[i].task_execution_time;
	    tasklist[i].task_deadline=wo.tl[i].task_deadline;
	    
	    std::cout << std::endl << "TASK " <<tasklist[i].task_id <<':'<< std::endl;
	    std::cout << "- posizione: " << tasklist[i].task_position[0] <<' '<< tasklist[i].task_position[1]<<' '<< tasklist[i].task_position[2] << std::endl;
	    std::cout << "- tipo: " << tasklist[i].task_type << std::endl;
	    std::cout << "- execution time: " << tasklist[i].task_execution_time << std::endl;
	    std::cout << "- deadline: " << tasklist[i].task_deadline << std::endl << std::endl;
    }
}

void task_assignment::createTaskCostMatrixFromParsedWorld(const Parsed_Agent& a)
{   
    task_cost_matrix.insert(make_pair(my_id,a.agent_task_cost_vector));
    
    task_cost_vector app;
    
    for (unsigned int i=0;i<agents_id.size();i++)
    {
      
	if(agents_id[i]!=my_id)
	{
	    for (unsigned int j=0;j<tasks_id.size();j++)
	    {
		app.insert(make_pair(tasks_id[j],0)); //i vettori di costo degli altri robot inizializzati come nulli
	    }
	    
	    task_cost_matrix.insert(make_pair(agents_id[i],app));
	    
	    for (task_cost_vector::iterator it=app.begin();it!=app.end();++it)
	    {
		app.erase(it);
	    }
	}
    }
    
    agent_task_cost_vector=&task_cost_matrix.at(my_id);
	  
}


void task_assignment::inizializeTaskAssignmentMatrix()
{
      
    task_assignment_vector app;
    
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    app.insert(make_pair(tasks_id[j],(i==j))); //X identità
	}

	task_assignment_matrix.insert(make_pair(agents_id[i],app));
	
	for (task_assignment_vector::iterator it=app.begin();it!=app.end();++it)
	{
	    app.erase(it);
	}
    }
    
    agent_task_assignment_vector=&task_assignment_matrix.at(my_id);
}


void task_assignment::printTaskAssignmentMatrix()
{
    std::cout<<std::endl<<"TASK ASSIGNMENT MATRIX"<<std::endl;
    
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    std::cout<<task_assignment_matrix.at(agents_id[i]).at(tasks_id[j])<<' ';
	}
	
	std::cout<<std::endl;
    }
}


void task_assignment::printTaskCostMatrix()
{
    std::cout<<std::endl<<"TASK COST MATRIX"<<std::endl;
    
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    std::cout<<task_cost_matrix.at(agents_id[i]).at(tasks_id[j])<<' ';
	}
	
	std::cout<<std::endl;
    }
}


int task_assignment::ta_1()
{
     int a=-1;

     while(!converge)
     {
	    sleep(1);
	    
	    data_mutex.lock();
	    
	    fresh_data=false;
	    
	    for (unsigned int i=0;i<data_r.size();i++)
	    {
		if (!(data_r[i].agent_id==""))
		{
		    data_s.g += data_r[i].g; //dato valido
		}
	    }
	    
	    data_r.clear();

	    std::cout<<"Invio: "<<data_s.g<<std::endl;
	    
	    ta_communicator.send(data_s);
	    
	    
	    //std::cout<<">>> A:"<<data_r.agent_id<<" D:"<<data_r.g<<" | "<<"A:"<<data_s.agent_id<<" D:"<<data_s.g<<" <<<"<<std::endl;
	    
	    if (data_s.g>3)
	    {
		  converge=true;//simulo un tempo per convergere, la convergenza è se la X è uguale per tutti in realtà
	    }
	    
	    data_mutex.unlock();
     }
     
     ta_communicator.send(data_s);
     
     for(unsigned int j=0;j<tasks_id.size();j++)
     {
	    if(agent_task_assignment_vector->at(tasks_id[j]) == true) a=j;
     }
     
     return a;
}


bool task_assignment::task_assignment_algorithm()
{     
     int a=ta_1();
     
     if (a==-1) return false;
     
     current_task=tasklist[a];
     return true;
}


void task_assignment::run_plugin()
{
    if (not_started)
    {
	 ta_communicator.init(my_id);
	 ta_communicator.start_threads();
	 not_started=false;
    }

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
	    std::cout<<"CURRENT TASK: "<<current_task.task_id<<", position: ["<<current_task.task_position[0]<<' '<<current_task.task_position[1]<<' '<<current_task.task_position[2]<<"], cost: "<<agent_task_cost_vector->at(current_task.task_id)<<std::endl;
	    task_assigned=true;
	    speed=1;
	}
     }
}