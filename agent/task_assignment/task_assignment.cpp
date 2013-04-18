//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>


using namespace task_assignment_namespace;

  
task_assignment :: task_assignment(const Parsed_World& world, const Parsed_Agent& agent, simulation_time& time, std::map< transition, Events >& events, const std::map< std::string, transition >& events_to_index, const std::map< std::string, task_assignment_task >& tasks)
:time(time),my_id(agent.name),x0(agent.initial_states.at("X")),y0(agent.initial_states.at("Y")),events(events),events_to_index(events_to_index), tasks(tasks)
{
    
    std::shared_ptr<std::mutex> temp(new std::mutex);
    ptr_receive_mutex.swap(temp);
    
    createAgentIdAndTaskIdVectorFromParsedWorld(world);
    
    num_robot=agents_id.size();
    num_task=tasks_id.size();
    
    createAusiliarVariables();    
    
    std::cout<<tasks<<std::endl;
    
    createTaskCostMatrixFromParsedWorld(agent);
    inizializeTaskAssignmentMatrix();
    
    
    task_assignment_algorithm=world.task_assignment_algorithm;
    
    my_task="";
    
    task_assigned=false;
    task_started=false;
    stop=false;
    speed=0;
    omega=0;
    omega_dubins=0;
    
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
	done_task[tasks_id.at(i)]=false;
    }
    //std::cout<<std::endl;
    
    for(unsigned int i=0;i<num_robot;i++)
    {
	busy_robots[agents_id.at(i)]=false;
	taken_tasks[agents_id.at(i)]="";
	previous_taken_tasks[agents_id.at(i)]="";
	std::vector<double> a;
	a.push_back(0);
	a.push_back(0);
	a.push_back(0);
	positions[agents_id.at(i)]=a;
	
	for(unsigned int j=0;j<num_task;j++)
	{
	      others_subgradient[agents_id.at(i)].push_back(0);
	}
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
    
    set_charge=-0.05;
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

 
void task_assignment ::createAusiliarVariables()
{ 
    for (unsigned int i=0; i< tasks.size();i++)
    {	
	    if(reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i)).getState())->task_type==2)
	    {
		  periodic_tasks_time[tasks_id.at(i)]=-150;
		  elapsed_times[tasks_id.at(i)]=0;
	    }
	    
	    DL.push_back(reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i)).getState())->task_deadline);
	    remaining_times_to_deadline[tasks_id.at(i)]=0;
    }
}

 
void task_assignment ::createTaskCostMatrixFromParsedWorld(const Parsed_Agent& a)
{   
    task_cost_matrix[my_id]=a.agent_task_cost_vector;
    
    task_cost_vector app;
    
    for (unsigned int i=0;i<num_robot;i++)
    {
      
	if(agents_id.at(i)!=my_id)
	{
	    for (unsigned int j=0;j<num_task;j++)
	    {
		app[tasks_id.at(j)]=0; //i vettori di costo degli altri robot inizializzati come nulli
	    }
	    
	    task_cost_matrix[agents_id.at(i)]=app;
	    
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
	    temp[tasks_id.at(j)]=false;
	}

	task_assignment_matrix[agents_id.at(i)]=temp;
	
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


void task_assignment::reset_mu_T()
{
	for (unsigned int i=0;i<num_task;i++)
	{
		mu_T.at(i)=0;
	}
}


unsigned int task_assignment::count_undone_task()
{
	unsigned int a=num_task-recharge_is_present();
  
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
		//std::cout<<"a="<<a<<'-'<<busy_robots.at(agents_id.at(i))<<'=';
		a -= busy_robots.at(agents_id.at(i));
		//std::cout<<a<<std::endl;
	}
	
	return a;
}


bool task_assignment::recharge_is_present()
{
	for (unsigned int i=0;i<num_task;i++)
	    if (tasks_id.at(i)=="RECHARGE") return true;
	    
	return false;
}


double task_assignment::norm2(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}


void task_assignment::compute_speeds(double x_t,double y_t)
{
	 omega=-1*sin(theta.value()-atan2(y_t-y.value(),x_t-x.value()));
	 	 
	 if((my_task!="TASK_ASSIGNMENT_FAILED") && (my_task!=""))
	 {
	      speed=((distance_to_target()>2)?0.2:(distance_to_target()/10))+0.05;
	 }
	 else
	 {
	      speed=((norm2(x.value(),y.value(),x0,y0)>2)?0.2:(norm2(x.value(),y.value(),x0,y0)/10))+0.05;
	 }
	 	 
// 	 if(distance_to_target()>0.1) avoid_collision();
	 	 
	 if(fabs(omega)>max_omega) omega_dubins=copysign(max_omega,omega);
	 else omega_dubins=omega;
}

void task_assignment::avoid_collision()
{
	double min_d=INF;
	for(unsigned int i=0;i<num_robot;i++)
	{
	      if(agents_id.at(i)!=my_id)
	      {
		      double d=norm2(x.value(),y.value(),positions.at(agents_id.at(i)).at(0),positions.at(agents_id.at(i)).at(1));
		      min_d=std::min(d,min_d);
		      omega += ((2)/(d))*sin(theta.value()-atan2(positions.at(agents_id.at(i)).at(1)-y.value(),positions.at(agents_id.at(i)).at(0)-x.value()));
	      }
	}
	
	speed = speed-((min_d<1.2)?0.15:0);
}


void task_assignment::receive_from_others()
{
	std::vector<subgradient_packet>& data_receive = *(std::vector<subgradient_packet>*)ta_communicator->get_data();
  
	ptr_receive_mutex->lock();
		
	fresh_data=false;
	
	for (unsigned int i=0;i<data_receive.size();i++)
	{
	    subgradient_task_packet temp = *(subgradient_task_packet*)data_receive.at(i).get_data();
	    agent_id name = data_receive.at(i).agent_id;
	    
	    if (!(name ==""))
	    {
		
		  for (unsigned int j=0;j<num_task;j++)
		  {
			others_subgradient[name].at(j) =  temp.subgradient.at(j);
		  }
		  
		  taken_tasks.at(name)="";
		  
		  if (data_receive.at(i).taken_task!="")
		  {
			done_task.at(data_receive.at(i).taken_task)=true;
			agent_task_cost_vector->at(data_receive.at(i).taken_task)=INF;
			copy_cost_vector_to_C();
			std::cout<<"task "<<data_receive.at(i).taken_task<<" e' preso"<<std::endl;
			taken_tasks.at(name)=data_receive.at(i).taken_task;
			
			if (reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(taken_tasks.at(name)).getState())->task_type==2)
			{
				periodic_tasks_time.at(taken_tasks.at(name))=time;
			}
		  }
		  
		  if (data_receive.at(i).busy)
		  {
			busy_robots.at(name)=true;
			std::cout<<"robot "<<name<<" e' occupato, "<<data_receive.at(i).busy<<std::endl;
		  }
		  else
		  {
			busy_robots.at(name)=false;
			std::cout<<"robot "<<name<<" e' libero, "<<data_receive.at(i).busy<<std::endl;
		  }
		  
		  std::vector<double> appv;
		  appv.push_back(data_receive.at(i).x);
		  appv.push_back(data_receive.at(i).y);
		  appv.push_back(data_receive.at(i).theta);;
		  positions[name]=appv;
		  appv.clear();
	    }
	    

	}
	
	
	for (unsigned int i=0;i<num_robot;i++)
	{
		if (previous_taken_tasks.at(agents_id.at(i))=="RECHARGE" && taken_tasks.at(agents_id.at(i))=="") done_task.at("RECHARGE")=false;
		previous_taken_tasks.at(agents_id.at(i))=taken_tasks.at(agents_id.at(i));
	}
	
	
	data_receive.clear();
	
	ptr_receive_mutex->unlock();
}


void task_assignment ::run_plugin()
{
      double dec=time-floor(time);

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

      
      std::cout<<"CARICA: "<<charge_.value()<<'%'<<std::endl;

      if(charge_.value() <= 0)
      {
	  std::cout<<"BATTERIA SCARICA... SHUTDOWN..."<<std::endl;
	  set_charge=0;
	  speed=0;
	  omega=0;
	  omega_dubins=0;
	  stop=true;
      }
            
      bool a=(fabs(dec-0.0) < 0.001)||(fabs(dec-0.5) < 0.001);
      if(a)
      {
		std::cout<<"[TA - "<<time<<']'<<std::endl;
		if(!stop)
		{
			update_distance_vector();
			update_remaining_times();
		      
			if (task_assigned)
			{
			    if (events.at(events_to_index.at("REACHED"))==Events::_TRUE)
			    {
				if(!task_started)
				{
					ptr_subgradient_packet.get()->busy=true;
					busy_robots.at(my_id)=true;
					ptr_subgradient_packet.get()->taken_task=my_task;
					task_started=true;
					//executing=true
					initial_time=time;
					std::cout<<"TASK "<<my_task<<" INIZIATO A "<<initial_time<<std::endl;
				
					speed=0;
					omega=0;
					omega_dubins=0;
				}
				
				if(my_task=="RECHARGE")
				{
				    if(charge_.value()>=100)set_charge=0;
				    else if(charge_.value()>=97)set_charge=0.1;
				         else set_charge=2;
				}
				else set_charge=-0.1;
			    }
			    else set_charge=-0.05;
			    
			    if(task_made())
			    {
			      
				if(reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task).getState())->task_type==2)
				{
				    periodic_tasks_time.at(my_task)=time;
				}
				
				if(my_task=="RECHARGE")
				{
				    std::cout<<"RICARICA EFFETTUATA"<<" T:"<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at("RECHARGE").getState())->task_execution_time<<std::endl;
				    done_task.at("RECHARGE")=false;
				}
				else
				{
				    std::cout<<"TASK "<<my_task<<" FINITO A "<<time<<std::endl;
				    
				    done_task.at(my_task)=true;
				}
				
				//executing=false;
				
				agent_task_cost_vector->at(my_task)=INF;
		      
				copy_cost_vector_to_C();
				
				task_assigned=false;
				speed=0;
				omega=0;
				omega_dubins=0;
				
				ptr_subgradient_packet.get()->taken_task="";
				ptr_subgradient_packet.get()->busy=false;
				
				busy_robots.at(my_id)=false;
	  // 		      if (count_undone_task()==0) stop=true;
				
				converge=false;
				
				step=0;
				passi=0;
				reset_TA_vector();
				task_started=false;
		      
		      reset_mu_T();

				my_task="";
				reset_mu_T();
			    }
			}
			else
			{
				if (events.at(events_to_index.at("REACHED"))==Events::_TRUE)
				{
					converge=false;
				}
			}
			
			if (stop)
			{
			    setTaskStop(true);
			    return;
			}
			
			if(recharge_is_present() && my_task!="RECHARGE")
			{
				if(!busy_robots.at(my_id) && !done_task.at("RECHARGE") && charge_.value() <= 30)
				{
				    agent_task_cost_vector->at("RECHARGE")=-INF; //così da farlo per forza
				    copy_cost_vector_to_C();
				    converge=false;
				}
			}
			
			for(std::map<std::string,double>::const_iterator it=periodic_tasks_time.begin(); it!=periodic_tasks_time.end(); ++it)
			{
			    elapsed_times.at(it->first)=150-(time-periodic_tasks_time.at(it->first)); //periodo 150
			    
			    if(elapsed_times.at(it->first)<=0)
			    {
				agent_task_cost_vector->at(it->first)=base_cost_vector.at(it->first); //costo base
				done_task.at(it->first)=false;
				copy_cost_vector_to_C();
			    }
			    else std::cout<<it->first<<" RITORNA TRA "<<elapsed_times.at(it->first)<<std::endl;
			}
			  
			
			if (!converge)
			{
				if(task_assignment_algorithm==SUBGRADIENT)
				{
				    my_task=subgradient_algorithm();
				}
				
				if((my_task=="TASK_ASSIGNMENT_FAILED") || (my_task==""))
				{
				    set_charge=-0.05;
				    std::cout<<"NO TASK HAS BEEN ASSIGNED TO ME"<<std::endl;
				    task_assigned=false;
				    my_task_x=x0;
				    my_task_y=y0;

				}    
				else
				{
				    my_task_x=reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task).getState())->task_position[0];
				    my_task_y=reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task).getState())->task_position[1];
				    std::cout<<"CURRENT TASK: "<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task).getState())->id<<", position: ["<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task).getState())->task_position[0]<<' '<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task).getState())->task_position[1]<<' '<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task).getState())->task_position[2]<<"], cost: "<<agent_task_cost_vector->at(reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task).getState())->id)<<std::endl;
				    task_assigned=true;

				}
			}
			else ta_communicator->send();
			
			if(my_task!="RECHARGE" && count_undone_task()==0)
			{
				  std::cout<<"TASK ESAURITI"<<std::endl;
				  my_task="";
				  task_assigned=false;
				  my_task_x=x0;
				  my_task_y=y0;

				  reset_mu_T();
			}
		}
      }
      else
      {
	      receive_from_others();
	      ptr_subgradient_packet.get()->x=x.value();
	      ptr_subgradient_packet.get()->y=y.value();
	      ptr_subgradient_packet.get()->theta=theta.value();
	      ta_communicator->send();
	      //std::cout<<"MANDO x:"<<x.value()<<" y:"<<y.value()<<std::endl;
      }
      
      compute_speeds(my_task_x,my_task_y);
}