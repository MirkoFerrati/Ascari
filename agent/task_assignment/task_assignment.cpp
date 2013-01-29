//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

  
task_assignment :: task_assignment(const Parsed_World& world, const Parsed_Agent& agent, std::map< transition, events::value >& events, const std::map<std::string,transition>& events_to_index)
:my_id(agent.name),events(events),events_to_index(events_to_index)
{    
    std::shared_ptr<std::mutex> temp(new std::mutex);
    ptr_receive_mutex.swap(temp);
    
    createAgentIdAndTaskIdVectorFromParsedWorld(world);
    createTaskListFromParsedWorld(world);
    createTaskCostMatrixFromParsedWorld(agent);
    inizializeTaskAssignmentMatrix();
    
    task_assignment_algorithm=world.task_assignment_algorithm;
    
    task_assigned=false;
    stop=false;
    speed=0;
    
    //printTaskAssignmentMatrix();
    printTaskCostMatrix();
    
    converge=false;
    fresh_data=true;
    
    not_started=true;
    
    initialize_bilp_problem(task_cost_matrix);
}

 
void task_assignment ::initialize_bilp_problem(std::map<agent_id,task_cost_vector>& m)
{	
	int z=0;
	
	for (unsigned int i=0;i<agents_id.size();i++)
	{
	    for (unsigned int j=0;j<tasks_id.size();j++)
	    {
		 C.push_back(m.at(agents_id[i]).at(tasks_id[j]));
		 z++;
	    }
	}
    
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
	    std::cout<<task_cost_matrix.at(agents_id[i]).at(tasks_id[j])<<' ';
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

 
int task_assignment ::subgradient_algorithm()
{
      //TODO
      return 0;
}



int task_assignment ::solution_exchange_algorithm()
{    
     solution_exchange_packet data_send;

     data_send.agent_id = my_id;
     
     std::vector<solution_exchange_packet>& data_receive = *(std::vector<solution_exchange_packet>*)ta_communicator->get_data();
         
     int a=-1;
     
     unsigned int w=0;
     
     unsigned int passi=1;
     
     std::vector<bool> solution;
     
     while(!converge && !s_interrupted)
     {
	    std::cout<<"----------------PASSO "<<passi<<"----------------"<<std::endl<<std::endl;
       
	    sleep(1);
	    
	    solution.clear();
	    
	    ta_problem.solve(solution);
     
	    copy_solution_to_TA_matrix(solution);
	    
	    ptr_receive_mutex->lock();
	    
	    fresh_data=false;
	    
	    for (unsigned int i=0;i<data_receive.size();i++)
	    {
		if (!(data_receive.at(i).agent_id ==""))
		{
		    task_assignment_namespace::task_assignment_matrix temp = *(task_assignment_namespace::task_assignment_matrix*)data_receive.at(i).get_data();
		    
		    if (task_assignment_matrix == temp) w++;
		}
	    }
	    
	    data_receive.clear();
	    
	    //do something with received data | this algorithm does not converge, it can be used for future implementation of another algorithm
	    //actually it converges if the agents optimal tasks are the same that you find with the bilp on the entire cost matrix
	    
	    data_send.set_data((void*)&task_assignment_matrix);
	    
	    ta_communicator->send(&data_send);	    
	    //ta_communicator->set_data((void*)&data_send);
	    //ptr_send_mutex->unlock();//send
	    
	    if (w==agents_id.size()-1)
	    {
		  std::cout<<"Converge, ";
		  printTaskAssignmentMatrix();
		  converge=true;
	    }
	    
	    if(!converge)printTaskAssignmentMatrix();
	    
	    w=0;
	    
	    ptr_receive_mutex->unlock();
	    
	    passi++;
     }
     
     ta_communicator->send(&data_send);

     for(unsigned int j=0;j<tasks_id.size();j++)
     {
	    if(agent_task_assignment_vector->at(tasks_id[j]) == true) a=j;
     }
     
     return a;
}


void task_assignment ::run_plugin()
{
  
     if (not_started)
     {
	  if(task_assignment_algorithm==1)
	  {		  
		  //data_send = new solution_exchange_packet(task_assignment_matrix);
		  //data_receive = new std::vector<solution_exchange_packet>();

		  //ta_communicator = new task_assignment_communicator<solution_exchange_packet,solution_exchange_packet>(*data_receive,*data_send,ptr_mutex,converge,agents_id.size()-1,my_id,fresh_data);
		  //ta_communicator = new task_assignment_communicator<solution_exchange_packet,solution_exchange_packet>(ptr_receive_mutex,ptr_send_mutex,converge,agents_id.size()-1,my_id,fresh_data);
		  ta_communicator = new task_assignment_communicator<solution_exchange_packet,solution_exchange_packet>(ptr_receive_mutex,converge,agents_id.size()-1,my_id,fresh_data);
	  
		  not_started=false;
	  }
	  
	  //ta_communicator = new task_assignment_communicator<task_assignment_packet_base,task_assignment_packet_base>(*data_receive,*data_send,ptr_mutex,converge,agents_id.size()-1,my_id,fresh_data);
     
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
	
	if(task_assignment_algorithm==0)
	{
	    a=subgradient_algorithm(); //TODO: fare funzione template per l'algoritmo
	}
	
	if(task_assignment_algorithm==1)
	{
	    a=solution_exchange_algorithm();
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
	    speed=0.1;
	}
     }
}