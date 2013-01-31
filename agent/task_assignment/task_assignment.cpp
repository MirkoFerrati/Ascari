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
    printTaskCostMatrix();
    
    update_costs_with_position();
    printTaskCostMatrix();
    
    update_costs_with_deadlines();
    printTaskCostMatrix();
    
    task_assigned=false;
    stop=false;
    speed=0;
    
    converge=false;
    fresh_data=true;
    
    not_started=true;
    
    initialize_bilp_problem();
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
 
void task_assignment ::initialize_bilp_problem()
{	
	copy_cost_matrix_to_cost_vector();
	
	ta_problem.initialize_problem("Task_Assignment",GLP_MIN,(int)agents_id.size(),(int)tasks_id.size(),C);
}

 
void task_assignment ::createAgentIdAndTaskIdVectorFromParsedWorld(const Parsed_World& wo)
{
    for (unsigned int i=0;i<wo.agents.size();i++)
      agents_id.push_back(wo.agents[i].name);  
    
    for (unsigned int i=0;i<wo.task_list.size();i++)
	tasks_id.push_back(wo.task_list[i].task_id);
}

 
void task_assignment ::createTaskListFromParsedWorld(const Parsed_World& wo)
{
    
    for (unsigned int i=0; i< wo.task_list.size();i++)
    {	
	    task app;
	    tasklist.push_back(app); //posso fare la push_back di tutto il task?
	    
	    tasklist[i].task_id=wo.task_list[i].task_id;
	    tasklist[i].task_position[0]=wo.task_list[i].task_position[0];
	    tasklist[i].task_position[1]=wo.task_list[i].task_position[1];
	    tasklist[i].task_position[2]=wo.task_list[i].task_position[2];
	    tasklist[i].task_type=wo.task_list[i].task_type;
	    tasklist[i].task_execution_time=wo.task_list[i].task_execution_time;
	    tasklist[i].task_deadline=wo.task_list[i].task_deadline;
	    
	    std::cout << std::endl << "TASK " <<tasklist[i].task_id <<':'<< std::endl;
	    std::cout << "- posizione: " << tasklist[i].task_position[0] <<' '<< tasklist[i].task_position[1]<<' '<< tasklist[i].task_position[2] << std::endl;
	    std::cout << "- tipo: " << tasklist[i].task_type << std::endl;
	    std::cout << "- execution time: " << tasklist[i].task_execution_time << std::endl;
	    std::cout << "- deadline: " << tasklist[i].task_deadline << std::endl << std::endl;
    }
}

 
void task_assignment ::createTaskCostMatrixFromParsedWorld(const Parsed_Agent& a)
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
	    
	    app.clear();
	}
    }
    
    agent_task_cost_vector=&task_cost_matrix.at(my_id);
	  
}

void task_assignment::update_costs_with_position()
{
    for (unsigned int i=0;i<tasks_id.size();i++) //controllo quali costi devo inviare
    {
	  if(task_cost_matrix.at(my_id).at(tasks_id.at(i))!=INF)
	      task_cost_matrix.at(my_id).at(tasks_id.at(i)) += distance_from_task(tasks_id.at(i));
    }
}


void task_assignment::update_costs_with_deadlines()
{
    for (unsigned int i=0;i<tasks_id.size();i++) //controllo quali costi devo inviare
    {
	  if(task_cost_matrix.at(my_id).at(tasks_id.at(i))!=INF)
	      task_cost_matrix.at(my_id).at(tasks_id.at(i)) /= time_to_deadline(tasks_id.at(i));
    }
}

 
void task_assignment ::inizializeTaskAssignmentMatrix()
{
      
    task_assignment_vector app;
    
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    app.insert(make_pair(tasks_id[j],false)); //X identità
	}

	task_assignment_matrix.insert(make_pair(agents_id[i],app));
	
	app.clear();
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
	    std::cout<<task_assignment_matrix.at(agents_id[i]).at(tasks_id[j])<<' ';
	}
	
	std::cout<<std::endl;
    }
    
    std::cout<<std::endl;
}

 
void task_assignment ::printTaskCostMatrix()
{
    std::cout<<"TASK COST MATRIX"<<std::endl;
    
    for(unsigned int i=0;i<agents_id.size();i++)
    {

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
	    if(task_cost_matrix.at(agents_id[i]).at(tasks_id[j])==INF)
	    {
		std::cout<<"INF"<<' ';
	    }
	    else
	    {
		std::cout<<task_cost_matrix.at(agents_id[i]).at(tasks_id[j])<<' ';
	    }
	}
	
	std::cout<<std::endl;
    }
    
    std::cout<<std::endl;
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
		ERR("attenzione, algoritmo per il task assignment non selezionato");
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
	int a;
	
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
	
	if(a==-1)
	{
	    ERR("attenzione, task assignment non riuscito");
	}
	else
	{
	    current_task=tasklist[a];
	    std::cout<<"CURRENT TASK: "<<current_task.task_id<<", position: ["<<current_task.task_position[0]<<' '<<current_task.task_position[1]<<' '<<current_task.task_position[2]<<"], cost: "<<agent_task_cost_vector->at(current_task.task_id)<<std::endl;
	    task_assigned=true;
	    speed=1;
	}
     }
}