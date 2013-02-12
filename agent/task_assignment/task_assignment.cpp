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
    createTaskListFromParsedWorld(world);
    createTaskCostMatrixFromParsedWorld(agent);
    inizializeTaskAssignmentMatrix();
    
    task_assignment_algorithm=world.task_assignment_algorithm;
    printTaskCostMatrix(task_cost_matrix);
    
    task_assigned=false;
    stop=false;
    speed=0;
    
    converge=false;
    fresh_data=true;
    
    not_started=true;
    
    initialize_bilp_problem();
}

 
void task_assignment ::initialize_bilp_problem()
{	
	copy_cost_matrix_to_cost_vector(task_cost_matrix);
	
	ta_problem.initialize_problem("Task_Assignment",GLP_MIN,(int)agents_id.size(),(int)tasks_id.size(),C);
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
    
    for (unsigned int i=0;i<agents_id.size();i++)
    {
      
	if(agents_id.at(i)!=my_id)
	{
	    for (unsigned int j=0;j<tasks_id.size();j++)
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
    
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    temp.insert(std::make_pair(tasks_id.at(j),false));
	}

	task_assignment_matrix.insert(std::make_pair(agents_id.at(i),temp));
	
	temp.clear();
    }
    
    agent_task_assignment_vector=&task_assignment_matrix.at(my_id);
}

 
void task_assignment ::printTaskAssignmentMatrix()
{
    std::cout<<"TASK ASSIGNMENT MATRIX"<<std::endl;
    
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    std::cout<<task_assignment_matrix.at(agents_id.at(i)).at(tasks_id.at(j))<<' ';
	}
	
	std::cout<<std::endl;
    }
    
    std::cout<<std::endl;
}

 
void task_assignment ::printTaskCostMatrix(task_assignment_namespace::task_cost_matrix& C_matrix)
{
    std::cout<<"TASK COST MATRIX"<<std::endl;
    
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    if(C_matrix.at(agents_id.at(i)).at(tasks_id.at(j))==INF)
	    {
		std::cout<<"INF"<<' ';
	    }
	    else
	    {
		std::cout<<C_matrix.at(agents_id.at(i)).at(tasks_id.at(j))<<' ';
	    }
	}
	
	std::cout<<std::endl;
    }
    
    std::cout<<std::endl;
}


void task_assignment ::run_plugin()
{
  
     if (not_started)
     {
	  if(task_assignment_algorithm==SOLUTION_EXCHANGE)
	  {		  
		  std::shared_ptr<solution_exchange_packet> temp(new solution_exchange_packet);
		  ptr_solution_exchange_packet.swap(temp);
		  ta_communicator = new task_assignment_communicator<solution_exchange_packet,solution_exchange_packet>(ptr_receive_mutex,ptr_solution_exchange_packet.get(),converge,agents_id.size()-1,my_id,fresh_data);
		
		  std::cout<<"TASK ASSIGNMENT ALGORITHM: SOLUTION EXCHANGE"<<std::endl;
		  not_started=false;
	  }
	  
	  if(task_assignment_algorithm==SUBGRADIENT)
	  {
		  std::shared_ptr<subgradient_packet> temp(new subgradient_packet);
		  ptr_subgradient_packet.swap(temp);
		  ta_communicator = new task_assignment_communicator<subgradient_packet,subgradient_packet>(ptr_receive_mutex,ptr_subgradient_packet.get(),converge,agents_id.size()-1,my_id,fresh_data);
	  
		  std::cout<<"TASK ASSIGNMENT ALGORITHM: SUBGRADIENT"<<std::endl;
		  not_started=false;
	  }
	  
	  if(task_assignment_algorithm==COST_EXCHANGE)
	  {
		  std::shared_ptr<cost_exchange_packet> temp(new cost_exchange_packet);
		  ptr_cost_exchange_packet.swap(temp);
		  ta_communicator = new task_assignment_communicator<cost_exchange_packet,cost_exchange_packet>(ptr_receive_mutex,ptr_cost_exchange_packet.get(),converge,agents_id.size()-1,my_id,fresh_data);
	  
		  std::cout<<"TASK ASSIGNMENT ALGORITHM: COST EXCHANGE"<<std::endl;
		  not_started=false;
	  }
	    
	  if(task_assignment_algorithm==-1)
	  {
		std::cout<<"attenzione, algoritmo per il task assignment non selezionato"<<std::endl;;
	  }
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
	task_id a;
	
	if(task_assignment_algorithm==SUBGRADIENT)
	{
	    a=subgradient_algorithm(); //TODO: fare funzione template per l'algoritmo
	}
	
	if(task_assignment_algorithm==SOLUTION_EXCHANGE)
	{
	    a=solution_exchange_algorithm();
	}
       
        if(task_assignment_algorithm==COST_EXCHANGE)
	{
	    a=cost_exchange_algorithm();
	}
	
	if(a=="TASK_ASSIGNMENT_FAILED")
	{
	    std::cout<<"attenzione, task assignment non riuscito"<<std::endl;
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