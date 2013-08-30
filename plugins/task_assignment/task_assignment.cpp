//written by Alessandro Settimi
#include "task_assignment.h"
#include "task_assignment_parser_plugin.h"
#include "../agent/agent.h"
#include "task_assignment_plugin.h"

using namespace task_assignment_namespace;


task_assignment::task_assignment ( agent* a, Parsed_World* parse )
:time(a->time),my_id(a->identifier),events(a->events),events_to_index(a->events_to_index),objects(a->objects)
{    
    x0=parse->agents.front().initial_states.at("X");
    y0=parse->agents.front().initial_states.at("Y");
    initialize(*parse);
}

  
task_assignment :: task_assignment(const Parsed_World& world, const Parsed_Agent& agent, simulation_time& time, std::map< transition, Events >& events,
				   const std::map< std::string, transition >& events_to_index, const objects_container& objects)
:time(time),my_id(agent.name),x0(agent.initial_states.at("X")),y0(agent.initial_states.at("Y")),events(events),events_to_index(events_to_index), objects(objects)
{
    initialize(world);  
}


void task_assignment::initialize(const Parsed_World& world)
{
    std::shared_ptr<std::mutex> temp(new std::mutex);
    ptr_receive_mutex.swap(temp);
    
    createAgentIdAndTaskIdVectorFromParsedWorld(world);
    
    num_robot=agents_id.size();
    num_task=tasks_id.size();
    
    createTaskCostMatrixFromParsedWorld(world.agents.front());
    inizializeTaskAssignmentMatrix();
    
    task_assignment_algorithm=reinterpret_cast<task_assignment_parsed_world*>(world.parsed_items_from_plugins[0])->task_assignment_algorithm;
    
    my_task="";
    my_task_x=x0;
    my_task_y=y0;
    
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
    
    initialize_assignment_problem();
    
    step=0;
    
    alpha=-1;
    
    if(recharge_is_present()) set_charge=-0.05;
    else set_charge=0;
    
    lambda_u=0;
}

 
void task_assignment ::initialize_assignment_problem()
{	
	copy_cost_vector_to_C();
	
	ta_problem.initialize_problem("Task_Assignment",GLP_MIN,(int)num_task,C);
}

 
void task_assignment ::createAgentIdAndTaskIdVectorFromParsedWorld(const Parsed_World& world)
{
  auto wo=reinterpret_cast<task_assignment_parsed_world*>(world.parsed_items_from_plugins[0]);    
    for (unsigned int i=0;i<wo->task_list.size();i++)
	tasks_id.push_back(wo->tasks_id.at(i));
    
    for (unsigned int i=0;i<wo->agents.size();i++)
      agents_id.push_back(wo->agents.at(i));  

}

 
void task_assignment ::createAusiliarVariables()
{ 
    for (unsigned int i=0; i< tasks.size();i++)
    {	
	    DL.push_back(reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i))->getState())->task_deadline);
	    remaining_times_to_deadline[tasks_id.at(i)]=0;
    }
}

 
void task_assignment ::createTaskCostMatrixFromParsedWorld(const Parsed_Agent& a)
{   
    task_cost_matrix[my_id]=(reinterpret_cast<task_assignment_parsed_agent*>(a.parsed_items_from_plugins[0]))->agent_task_cost_vector;
    
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
		if(tasks_id.at(i)!="RECHARGE") a -= (1-reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i))->getState())->available);
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


void task_assignment::compute_speeds(double x_t,double y_t)
{
	 lambda_u=0; //per la max_dist
	 
	 omega=-1*sin(theta.value()-atan2(y_t-y.value(),x_t-x.value())) + lambda_u;
	 	 
	 if((my_task!="TASK_ASSIGNMENT_FAILED") && (my_task!=""))
	 {
	      speed=((distance_to_target()>2)?0.2:(distance_to_target()/10));
	 }
	 else
	 {
	      speed=((norm2(x.value(),y.value(),x0,y0)>2)?0.2:(norm2(x.value(),y.value(),x0,y0)/10));
	 }
	 	 
// 	 if(distance_to_target()>0.1) avoid_collision();
	 
	 avoid_collision(omega_tilde);
	 
	 beta_p=-0.1;
	 
	 lambda_u += beta_p* (omega-omega_tilde);
	 	 
	 if(fabs(omega)>max_omega) omega_dubins=copysign(max_omega,omega);
	 else omega_dubins=omega;
}

void task_assignment::avoid_collision(double& a)
{
	a=0;
	double min_d=INF;
	for(unsigned int i=0;i<num_robot;i++)
	{
	      if(agents_id.at(i)!=my_id)
	      {
		      double d=norm2(x.value(),y.value(),positions.at(agents_id.at(i)).at(0),positions.at(agents_id.at(i)).at(1));
		      min_d=std::min(d,min_d);
		      if(d > 0) a += ((2)/(d))*sin(theta.value()-atan2(positions.at(agents_id.at(i)).at(1)-y.value(),positions.at(agents_id.at(i)).at(0)-x.value()));
	      }
	}
	
	a -= lambda_u;
	
	//speed = speed-((min_d<1.2)?0.15:0); //per rallentare vicino ad altri robot
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
	    
	    if (!(name =="") && (name!=my_id))
	    {
		
		  for (unsigned int j=0;j<num_task;j++)
		  {
			others_subgradient[name].at(j) =  temp.subgradient.at(j);
		  }
		  
		  taken_tasks.at(name)="";
		  
		  if (data_receive.at(i).taken_task!="")
		  {
			agent_task_cost_vector->at(data_receive.at(i).taken_task)=INF;
			copy_cost_vector_to_C();
// 			std::cout<<"task "<<data_receive.at(i).taken_task<<" e' preso"<<std::endl;
			taken_tasks.at(name)=data_receive.at(i).taken_task;
			if (my_task==taken_tasks.at(name)) converge=false;
		  }
		  
		  if (data_receive.at(i).busy)
		  {
			busy_robots.at(name)=true;
// 			std::cout<<"robot "<<name<<" e' occupato, "<<data_receive.at(i).busy<<std::endl;
		  }
		  else
		  {
			busy_robots.at(name)=false;
// 			std::cout<<"robot "<<name<<" e' libero, "<<data_receive.at(i).busy<<std::endl;
		  }
		  
		  std::vector<double> appv;
		  appv.push_back(data_receive.at(i).x);
// 		  appv.push_back(data_receive.at(i).y);
		  appv.push_back(data_receive.at(i).theta);;
		  positions[name]=appv;
		  appv.clear();
	    }
	    

	}
	
	
	for (unsigned int i=0;i<num_robot;i++)
	{
		previous_taken_tasks.at(agents_id.at(i))=taken_tasks.at(agents_id.at(i));
	}
	
	
	data_receive.clear();
	
	ptr_receive_mutex->unlock();
}


void task_assignment ::run_plugin()
{ 
	if (objects.objects.count(TA_PLUGIN_IDENTIFIER))
	{	    
	    for (auto object:objects.objects.at(TA_PLUGIN_IDENTIFIER))
	    {
		tasks[object->name]=reinterpret_cast<task_assignment_task*>(object);
		
 		//std::cout<<*(reinterpret_cast<task_assignment_task*>(object))<<std::endl;
	    }
	}


        if(!tasks.empty()) //assert
	{
		double dec=time-floor(time);

		if (not_started)
		{
		      createAusiliarVariables(); 
			  
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


		if(recharge_is_present()) std::cout<<"CARICA: "<<charge_.value()<<'%'<<std::endl;

		if(recharge_is_present() && charge_.value() <= 0)
		{
		    std::cout<<"BATTERIA SCARICA... SHUTDOWN..."<<std::endl;
		    set_charge=0;
		    speed=0;
		    omega=0;
		    omega_dubins=0;
		    stop=true;
		}
		      
		bool a=(fabs(dec-0.0) < 0.001)||(fabs(dec-0.5) < 0.001); //TODO: BEWARE OF REAL TIME!!
		if(a)
		{
			  std::cout<<"[TA - "<<time<<']'<<std::endl;
			  if(!stop)
			  {
				  update_distance_vector();
				  update_remaining_times();
				
				  if (task_assigned)
				  {
// 				      std::list<abstract_object*> app=objects.objects.at(TA_PLUGIN_IDENTIFIER);
// 				      for(auto i:app)if(i->name=="T2")i->print(std::cout);
				      
				      if (reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->executing && reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->owner==my_id)
				      {
					  if(!task_started)
					  {
						  ptr_subgradient_packet.get()->busy=true;
						  busy_robots.at(my_id)=true;
						  ptr_subgradient_packet.get()->taken_task=my_task;
						  task_started=true;
						  //executing=true
						  std::cout<<"TASK "<<my_task<<" INIZIATO A "<<time<<std::endl;
					  }
					  
					  if(my_task=="RECHARGE")
					  {
					      if(charge_.value()>=100)set_charge=0;
					      else if(charge_.value()>=97)set_charge=0.1;
						  else set_charge=2;
					  }
					  else if (recharge_is_present()) set_charge=-0.1;
				      }
				      else if (recharge_is_present()) set_charge=-0.05;
				      
				      if(task_made())
				      {
					  std::cout<<"TASK "<<my_task<<" FINITO A "<<time<<std::endl;
															  
					  //executing=false;
					  
					  agent_task_cost_vector->at(my_task)=INF;
				
					  copy_cost_vector_to_C();
					  
					  task_assigned=false;
					  
					  ptr_subgradient_packet.get()->taken_task="";
					  
					  ptr_subgradient_packet.get()->busy=false;
					  busy_robots.at(my_id)=false;
					  
					  converge=false;
					  
					  step=0;
					  passi=0;
					  reset_TA_vector();
					  task_started=false;

					  my_task="";
					  reset_mu_T();
				      }
				  }
				  else
				  {
					  if (my_task=="" && events.at(events_to_index.at("REACHED"))==Events::_TRUE)
					  {
						  converge=false;
						  reset_mu_T();
					  }
				  }
				  
				  if (stop)
				  {
				      setTaskStop(true);
				      return;
				  }
				  
				  if(recharge_is_present() && my_task!="RECHARGE")
				  {
					  if(!busy_robots.at(my_id) && reinterpret_cast<const task_assignment_namespace::task*>(tasks.at("RECHARGE")->getState())->available && charge_.value() <= 30)
					  {
					      agent_task_cost_vector->at("RECHARGE")=-INF; //così da farlo per forza
					      copy_cost_vector_to_C();
					      converge=false;
					  }
				  } 
				  
				  for (unsigned int i=0;i<num_task;i++)
				  {
					  if (reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i))->getState())->task_type==2)
					  {
						  if (reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i))->getState())->available)
						  {
							  agent_task_cost_vector->at(tasks_id.at(i))=base_cost_vector.at(tasks_id.at(i));
							  copy_cost_vector_to_C();
							  converge=false;
	// 							  std::cout<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i)).getState())->id<<" BASE:"<<base_cost_vector<<std::endl;
	// 							  std::cout<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i)).getState())->id<<" NEW:"<<*agent_task_cost_vector<<std::endl;
						  }
					  }
				  }
				  
				  if (!converge)
				  {
					  if(task_assignment_algorithm==SUBGRADIENT)
					  {
					      my_task=subgradient_algorithm();
					  }
					  
					  if((my_task=="TASK_ASSIGNMENT_FAILED") || (my_task==""))
					  {
					      if (recharge_is_present()) set_charge=-0.05;
					      std::cout<<"NO TASK HAS BEEN ASSIGNED TO ME"<<std::endl;
					      task_assigned=false;
					      my_task_x=x0;
					      my_task_y=y0;

					  }    
					  else
					  {
					      my_task_x=reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->task_position[0];
					      my_task_y=reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->task_position[1];
					      std::cout<<"CURRENT TASK: "<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->id<<", position: ["<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->task_position[0]<<' '<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->task_position[1]<<' '<<reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->task_position[2]<<"], cost: "<<agent_task_cost_vector->at(reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(my_task)->getState())->id)<<std::endl;
					      task_assigned=true;

					  }
				  }
				  else ta_communicator->send();
				  
				  if(!busy_robots.at(my_id) && my_task!="RECHARGE" && count_undone_task()==0 && events.at(events_to_index.at("REACHED"))==Events::_TRUE)
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
		//std::cout<<"speed= "<<speed<<" omega= "<<omega<<" omega_dubins "<<omega_dubins<<" set_charge= "<<set_charge<<std::endl;
	}      
}