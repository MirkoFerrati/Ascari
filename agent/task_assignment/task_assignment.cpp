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
    
    my_task="";
    
    charge=100;
    if(my_id=="AGENTE1") charge=100;
    if(my_id=="AGENTE2") charge=100;
    if(my_id=="AGENTE3") charge=50;
    
    task_assigned=false;
    task_started=false;
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
	if(tasks_id.at(i)!="RECHARGE")done_task.insert(std::make_pair(tasks_id.at(i),false));
    }
    //std::cout<<std::endl;
    
    for(unsigned int i=0;i<num_robot;i++)
    {
	busy_robots.insert(std::make_pair(agents_id.at(i),false));
    }
    
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
    
    tasklist = wo.task_list;
  
    for (unsigned int i=0; i< tasklist.size();i++)
    {	
	    //tasklist.insert(std::make_pair(tasks_id.at(i),wo.task_list.at(tasks_id.at(i))));
	    
	    std::cout << std::endl << "TASK " << tasks_id.at(i) <<':'<< std::endl;
	    std::cout << "- posizione: " << tasklist.at(tasks_id.at(i)).task_position[0] <<' '<< tasklist.at(tasks_id.at(i)).task_position[1]<<' '<< tasklist.at(tasks_id.at(i)).task_position[2] << std::endl;
	    std::cout << "- tipo: " << tasklist.at(tasks_id.at(i)).task_type << std::endl;
	    std::cout << "- execution time: " << tasklist.at(tasks_id.at(i)).task_execution_time << std::endl;
	    std::cout << "- deadline: " << tasklist.at(tasks_id.at(i)).task_deadline << std::endl << std::endl;
    
	    if(tasklist.at(tasks_id.at(i)).task_type==2)
	    {
		  periodic_tasks_time.insert(std::make_pair(tasks_id.at(i),0));
		  elapsed_times.insert(std::make_pair(tasks_id.at(i),0));
	    }
	    
	    DL.push_back(tasklist.at(tasks_id.at(i)).task_deadline);
	    remaining_times_to_deadline.insert(std::make_pair(tasks_id.at(i),0));
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
    
    base_cost_vector=task_cost_matrix.at(my_id);
    
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
	    std::cout<<"INF"<<'|';
	}
	else
	{
	    std::cout<<C.at(j)<<'|';
	}
    }
    
    std::cout<<std::endl;
}


void task_assignment::reset_TA_vector()
{
	for (unsigned int i=0;i<num_task;i++)
	{
		agent_task_assignment_vector->at(tasks_id.at(i))=0;
	}
}


unsigned int task_assignment::count_undone_task()
{
	unsigned int a=num_task-1; //TODO:se c'è il RECHARGE ci vuole il -1, sennò no
  
	for (unsigned int i=0;i<num_task;i++)
	{
		if(tasks_id.at(i)!="RECHARGE") a -= done_task.at(tasks_id.at(i));
	}
	
	return a;
}

unsigned int task_assignment::count_free_robots()
{
	unsigned int a=num_robot;
	
	for (unsigned int i=0;i<num_robot;i++)
	{
		std::cout<<"a="<<a<<'-'<<busy_robots.at(agents_id.at(i))<<'=';
		a -= busy_robots.at(agents_id.at(i));
		std::cout<<a<<std::endl;
	}
	
	return a;
}


void task_assignment ::run_plugin()
{
      if(!stop)
      {
	      if (not_started)
	      {
		    
		    if(task_assignment_algorithm==SUBGRADIENT)
		    {
			    std::shared_ptr<subgradient_packet> temp(new subgradient_packet);
			    ptr_subgradient_packet.swap(temp);
			    ptr_subgradient_packet.get()->busy=false;
			    ptr_subgradient_packet.get()->taken_task="";
			    ta_communicator = new task_assignment_communicator<subgradient_packet,subgradient_packet>(ptr_receive_mutex,ptr_subgradient_packet.get(),num_robot-1,my_id,fresh_data);
		    
			    std::cout<<"TASK ASSIGNMENT ALGORITHM: SUBGRADIENT"<<std::endl;
			    //start thread
			    not_started=false;
		    }
		      
		    if(task_assignment_algorithm==-1)
		    {
			  std::cout<<"attenzione, algoritmo per il task assignment non selezionato"<<std::endl;;
		    }
	      }
	      else
	      {
		    std::cout<<"CARICA: "<<charge<<'%'<<std::endl;
		    if(charge > 0)
		    {
			charge-=0.01;
		    }
		    else
		    {
			std::cout<<"BATTERIA SCARICA... SHUTDOWN..."<<std::endl;
			stop=true;
		    }
		    
		    update_distance_vector();
		    update_remaining_times();
	      }
	      
	      
	    
	      if (task_assigned)
	      {
		
		  if (events.at(events_to_index.at("REACHED"))==Events::_TRUE && !task_started)
		  {
		      ptr_subgradient_packet.get()->busy=true;
		      busy_robots.at(my_id)=true;
		      ptr_subgradient_packet.get()->taken_task=my_task;
		      task_started=true;
		      tasklist.at(my_task).executing=true;
		      initial_time=time;
		      std::cout<<"TASK INIZIATO A "<<initial_time<<std::endl;
		      charge-=1;
		      speed=0;
		  }
		  
		  if(task_made())
		  {
		    
		      if(tasklist.at(my_task).task_type==2)
		      {
			  periodic_tasks_time.at(my_task)=time;
		      }
		      
		      if(my_task=="RECHARGE")
		      {
			  std::cout<<"RICARICA EFFETTUATA"<<std::endl;
			  charge=100;
		      }
		      else
		      {
			  std::cout<<"TASK FINITO A "<<time<<std::endl;
			  
			  done_task.at(my_task)=true;
		      }
		      
		      tasklist.at(my_task).executing=false;
		      
		      agent_task_cost_vector->at(my_task)=INF;
		      copy_cost_vector_to_C();
		      
		      task_assigned=false;
		      speed=0;
		      
		      ptr_subgradient_packet.get()->taken_task="";
		      ptr_subgradient_packet.get()->busy=false;
		      
		      busy_robots.at(my_id)=false;
// 		      if (count_undone_task()==0) stop=true;
		      
		      converge=false;
		      my_task="";
		      step=0;
		      passi=0;
		      reset_TA_vector();
		  }
		  
		  ta_communicator->send();
	      }
	      
	      if (stop)
	      {
		  setTaskStop(true);
		  return;
	      }
	      
	      if(charge <= 30)
	      {
		  agent_task_cost_vector->at("RECHARGE")=-100; //così da farlo per forza
		  copy_cost_vector_to_C();
	      }
	      
	      for(std::map<std::string,double>::const_iterator it=periodic_tasks_time.begin(); it!=periodic_tasks_time.end(); ++it)
	      {
		  if(elapsed_times.at(it->first)<=0)
		  {
		      agent_task_cost_vector->at(it->first)=base_cost_vector.at(it->first); //costo base
		      done_task.at(it->first)=false;
		      copy_cost_vector_to_C();
		  }
		  else
		  {
		      elapsed_times.at(it->first)=100-(time-periodic_tasks_time.at(it->first)); //periodo 50
		  }
	      }
		
	      
	      if (/*!converge &&*/ count_undone_task()!=0)
	      {
		      if(task_assignment_algorithm==SUBGRADIENT)
		      {
			  my_task=subgradient_algorithm();
		      }
		      
		      if((my_task=="TASK_ASSIGNMENT_FAILED") || (my_task==""))
		      {
			  std::cout<<"NO TASK HAS BEEN ASSIGNED TO ME"<<std::endl;
			  task_assigned=false;
			  speed=0;
		      }    
		      else
		      {
			  my_task_x=tasklist.at(my_task).task_position[0];
			  my_task_y=tasklist.at(my_task).task_position[1];
			  std::cout<<"CURRENT TASK: "<<tasklist.at(my_task).id<<", position: ["<<tasklist.at(my_task).task_position[0]<<' '<<tasklist.at(my_task).task_position[1]<<' '<<tasklist.at(my_task).task_position[2]<<"], cost: "<<agent_task_cost_vector->at(tasklist.at(my_task).id)<<std::endl;
			  task_assigned=true;
			  speed=0.1;
		      }
	      }
	      
	      if(count_undone_task()==0)
	      {
			my_task_x=x0;
			my_task_y=y0;
	      }
      }
}