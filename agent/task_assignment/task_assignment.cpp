//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

  
task_assignment :: task_assignment(const Parsed_World& world, const Parsed_Agent& agent, simulation_time& time, std::map< transition, Events >& events, const std::map<std::string,transition>& events_to_index)
:time(time),my_id(agent.name),x0(agent.initial_states.at("X")),y0(agent.initial_states.at("Y")),events(events),events_to_index(events_to_index)
{        
    std::shared_ptr<std::mutex> temp(new std::mutex);
    ptr_receive_mutex.swap(temp);
    
    createAgentIdAndTaskIdVectorFromParsedWorld(world);
    
    num_robot=agents_id.size();
    num_task=tasks_id.size();
    
    createTaskListFromParsedWorld(world);
    createTaskCostMatrixFromParsedWorld(agent);
    inizializeTaskAssignmentMatrix();
    
    
    task_assignment_algorithm=world.task_assignment_algorithm;
    
    task_assigned=false;
    stop=false;
    speed=0;
    
    converge=false;
    fresh_data=true;
    
    not_started=true;
    
    //subgradient algorithm stuff
    //std::string s=my_id;
    //s.replace(s.begin(),s.begin()+6,"");
    //unsigned int j = atoi(s.c_str());
    //std::cout<<"s:"<<s<<','<<"j:"<<j<<std::endl;
    
    //std::cout<<"e_i= |";
    for( unsigned int i=0;i<num_task;i++)
    {
        subgradient.push_back(0);
	total_subgradient.push_back(0);
	//e_i.push_back(i==(j-1));
	//std::cout<<e_i.at(i)<<'|';
	e_i.push_back(1);
	mu_T.push_back(0);
	C.push_back(0);
	D.push_back(0);
	F.push_back(0);
    }
    //std::cout<<std::endl;
    
    copy_cost_vector_to_C();
    
    initialize_assignment_problem();
    
    empty_task.task_position[0]=0;
    empty_task.task_position[1]=0;
    empty_task.task_position[2]=0;
    empty_task.task_type=0;
    empty_task.task_execution_time=0;
    empty_task.task_deadline=0;
    
    step=0;
    
    alpha=-1;
}

 
void task_assignment ::initialize_assignment_problem()
{	
	copy_cost_vector_to_C();
	
	ta_problem.initialize_problem("Task_Assignment",GLP_MIN,(int)num_task,C);
}

 
void task_assignment ::createAgentIdAndTaskIdVectorFromParsedWorld(const Parsed_World& wo)
{
    for (unsigned int i=0;i<wo.agents.size();i++)
      agents_id.push_back(wo.agents.at(i).name);  
    
    for (unsigned int i=0;i<wo.task_list.size();i++)
	tasks_id.push_back(wo.tasks_id.at(i));
}

 
void task_assignment ::createTaskListFromParsedWorld(const Parsed_World& wo)
{
    
    for (unsigned int i=0; i< wo.task_list.size();i++)
    {	
	    tasklist.insert(std::make_pair(tasks_id.at(i),wo.task_list.at(tasks_id.at(i))));
	    
	    std::cout << std::endl << "TASK " << tasks_id.at(i) <<':'<< std::endl;
	    std::cout << "- posizione: " << tasklist.at(tasks_id.at(i)).task_position[0] <<' '<< tasklist.at(tasks_id.at(i)).task_position[1]<<' '<< tasklist.at(tasks_id.at(i)).task_position[2] << std::endl;
	    std::cout << "- tipo: " << tasklist.at(tasks_id.at(i)).task_type << std::endl;
	    std::cout << "- execution time: " << tasklist.at(tasks_id.at(i)).task_execution_time << std::endl;
	    std::cout << "- deadline: " << tasklist.at(tasks_id.at(i)).task_deadline << std::endl << std::endl;
    }
}

 
void task_assignment ::createTaskCostMatrixFromParsedWorld(const Parsed_Agent& a)
{   
    task_cost_matrix.insert(std::make_pair(my_id,a.agent_task_cost_vector));
    
    task_cost_vector app;
    
    for (unsigned int i=0;i<num_robot;i++)
    {
      
	if(agents_id.at(i)!=my_id)
	{
	    for (unsigned int j=0;j<num_task;j++)
	    {
		app.insert(std::make_pair(tasks_id.at(j),0)); //i vettori di costo degli altri robot inizializzati come nulli
	    }
	    
	    task_cost_matrix.insert(std::make_pair(agents_id.at(i),app));
	    
	    app.clear();
	}
    }
    
    agent_task_cost_vector=&task_cost_matrix.at(my_id);
	  
}

 
void task_assignment ::inizializeTaskAssignmentMatrix()
{
      
    task_assignment_vector temp;
    
    for(unsigned int i=0;i<num_robot;i++)
    {

	for(unsigned int j=0;j<num_task;j++)
	{
	    temp.insert(std::make_pair(tasks_id.at(j),false));
	}

	task_assignment_matrix.insert(std::make_pair(agents_id.at(i),temp));
	
	temp.clear();
    }
    
    agent_task_assignment_vector=&task_assignment_matrix.at(my_id);
}

 
void task_assignment ::printTaskAssignmentVector()
{
    std::cout<<"TASK ASSIGNMENT VECTOR"<<std::endl;
    
    for(unsigned int j=0;j<num_task;j++)
    {
	    std::cout<<agent_task_assignment_vector->at(tasks_id.at(j))<<' ';
    }
    
    std::cout<<std::endl;
}

 
void task_assignment ::printTaskCostVector()
{
    std::cout<<"TASK COST VECTOR: |";
    

    for(unsigned int j=0;j<num_task;j++)
    {
	if(C.at(j)==INF)
	{
	    std::cout<<"INF"<<' ';
	}
	else
	{
	    std::cout<<C.at(j)<<'|';
	}
    }
    
    std::cout<<std::endl;
}


void task_assignment ::run_plugin()
{
  
     if (not_started)
     {
	  
	  if(task_assignment_algorithm==SUBGRADIENT)
	  {
		  std::shared_ptr<subgradient_packet> temp(new subgradient_packet);
		  ptr_subgradient_packet.swap(temp);
		  ta_communicator = new task_assignment_communicator<subgradient_packet,subgradient_packet>(ptr_receive_mutex,ptr_subgradient_packet.get(),num_robot-1,my_id,fresh_data);
	  
		  std::cout<<"TASK ASSIGNMENT ALGORITHM: SUBGRADIENT"<<std::endl;
		  not_started=false;
	  }
	    
	  if(task_assignment_algorithm==-1)
	  {
		std::cout<<"attenzione, algoritmo per il task assignment non selezionato"<<std::endl;;
	  }
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
     
     if (stop)
     {
	setTaskStop(true);
	return;
     }
     
    task_id a;
    
    if (!stop && !converge)
    {
	    if(task_assignment_algorithm==SUBGRADIENT)
	    {
		a=subgradient_algorithm();
	    }
	    
	    if(a=="TASK_ASSIGNMENT_FAILED")
	    {
		std::cout<<"NO TASK HAS BEEN ASSIGNED TO ME"<<std::endl;
		current_task=empty_task;
		task_assigned=false;
		speed=0;
	    }
		    
	    else
	    {
		current_task=tasklist.at(a);
		std::cout<<"CURRENT TASK: "<<a<<", position: ["<<current_task.task_position[0]<<' '<<current_task.task_position[1]<<' '<<current_task.task_position[2]<<"], cost: "<<agent_task_cost_vector->at(a)<<std::endl;
		task_assigned=true;
		speed=1;
	    }
    }
}