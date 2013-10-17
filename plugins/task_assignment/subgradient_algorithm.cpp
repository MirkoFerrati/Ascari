//written by Alessandro Settimi
#include "task_assignment.h"


using namespace task_assignment_namespace;

bool task_assignment::convergence_control_routine()
{
     bool a=1;
     unsigned int iter=0;
     unsigned int control=0;
     
     unsigned int fr = count_free_robots();
     unsigned int ut = count_undone_task();
     
     std::cout<<"FREE ROBOTS: "<<fr<<std::endl;
     std::cout<<"UNDONE TASK: "<<ut<<std::endl;
     
     if ( fr < ut ) control=fr;
     else control=ut;

     std::cout<<"TASK DONE or ASSIGNED: "<<num_task-ut-recharge_is_present()<<" CONTROL :"<<control<<std::endl;
     
     for (unsigned int i=0;i<num_task;i++)
     {
	    if (fabs(total_subgradient.at(i)) < 0.01) iter++;
     }

     std::cout<<"zeri in SIGMA: "<<iter<<std::endl;
     
     if(iter==control)
     {
	  step++;
	  std::cout<<"passi stabili: "<<step<<std::endl;
     }
     
     if(!(iter==control))
     {
	  step=0;
     }
      
     a = a && (step>50);
	 
     if(a)
     {
	    std::cout<<"CONVERGE"<<std::endl;
     }
	  
     return a;
}


bool find_mininum_pair_task_id_cost(std::pair<task_id,task_cost> i,std::pair<task_id,task_cost> j)
{
      return i.second < j.second;
}

void task_assignment ::control_print()
{
	std::cout<<"F: |";
	for (unsigned int i=0;i<num_task;i++)
	{
	      std::cout<<F.at(i)<<'|';
	}
	std::cout<<std::endl<<std::endl;
	
	std::cout<<"D: |";
	for (unsigned int i=0;i<num_task;i++)
	{
	      std::cout<<D.at(i)<<'|';
	}
	std::cout<<std::endl<<std::endl;
	
	std::cout<<"C: |";
	for (unsigned int i=0;i<num_task;i++)
	{
	      std::cout<<C.at(i)<<'|';
	}
	std::cout<<std::endl<<std::endl;
	
	
	std::cout<<"SELECTED TASK: "<<selected_task<<std::endl<<std::endl;
	
	printTaskCostVector();
	
	printTaskAssignmentVector();
	
	std::cout<<"ALPHA="<<alpha<<std::endl;
		
	std::cout<<"mu_T = |";
	for (unsigned int i=0;i<num_task;i++)
	{
	      std::cout<<mu_T.at(i)<<'|';
	}
	std::cout<<std::endl;
			
	std::cout<<"Send => sigma_i = |";
	for (unsigned int i=0;i<num_task;i++)
	{
		std::cout<< subgradient.at(i) <<'|';
	}
	std::cout<<std::endl<<std::endl;
}
 
task_id task_assignment ::subgradient_algorithm()
{
	if(1)//!converge)
	{
		ptr_subgradient_packet.get()->agent_id=my_id;
		
		if(!busy_robots.at(my_id))
		{
		  
			std::cout<<"----------------PASSO "<<passi<<" ("<<time<<") ----------------"<<std::endl<<std::endl;

			
			for (unsigned int i=0;i<num_task;i++)
			{
			      F.at(i) = C.at(i) + D.at(i) + mu_T.at(i);
			      
			      if(tasks_id.at(i)!="RECHARGE" && (((!reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i))->getState())->executing && reinterpret_cast<const task_assignment_namespace::task*>(tasks.at(tasks_id.at(i))->getState())->available)))) F.at(i) += ((remaining_times_to_deadline.at(tasks_id.at(i))>50)?0:(-DL.at(i)));
			}
			

			  
			//ta_problem.set_cost_vector(F);
			
			
			//ta_problem.solve(solution);
			//***********************************
			double prev=0;
			int min=-1;
			
			for(int i=0;i<F.size();i++)
			{
			    if (prev > F[i])
			    {
				prev=F[i];
				min=i;
			    }
			    
			    solution.push_back(0);
			}
			
			if (min != -1)
			{
			    solution[min]=1;
			}
			//***********************************
			
			for(unsigned int j=0;j<num_task;j++)//copy_solution_to_TA_vector
			{
				agent_task_assignment_vector->at(tasks_id.at(j))=solution.at(j);
			}

			
			solution.clear();
		}
		
		selected_task="";
			
		for (unsigned int i=0;i<num_task;i++)
		{
		      if (agent_task_assignment_vector->at(tasks_id.at(i)) > 0.5)
			  selected_task = tasks_id.at(i);
		      
		      subgradient.at(i) = -(agent_task_assignment_vector->at(tasks_id.at(i)));
		      total_subgradient.at(i) = subgradient.at(i) + e_i.at(i);
		}
		
		
		receive_from_others();
		
		
		for (unsigned int j=0;j<num_task;j++)
		{
		      for (unsigned int h=0;h<num_robot;h++)
		      {
			    total_subgradient.at(j) +=  others_subgradient.at(agents_id.at(h)).at(j);
		      }
		}
		
		
		for (unsigned int i=0;i<num_robot;i++)
		{
			previous_taken_tasks.at(agents_id.at(i))=taken_tasks.at(agents_id.at(i));
		}
		
		
		
		std::cout<<"SIGMA = |"; 
		for (unsigned int i=0;i<num_task;i++)
		{
			std::cout<< total_subgradient.at(i) <<'|';
		}
		std::cout<<std::endl;
		
		alpha=compute_alpha();
		
		if(1) //TODO: RIFLETTERE SUL FATTO CHE MU_T NON CONVERGE SE CI SONO PIU' TASKS
		{
			for (unsigned int i=0;i<num_task;i++)
			{
			      mu_T.at(i) = mu_T.at(i) + alpha* total_subgradient.at(i);
			}
		}
			
			
		if(!busy_robots.at(my_id))
		{
				
			ptr_subgradient_packet.get()->data.subgradient=subgradient;
						
			control_print();
						
			//converge=convergence_control_routine();
			
			passi++;
		}
		
		ptr_subgradient_packet.get()->x=x.value();
		ptr_subgradient_packet.get()->y=y.value();
		ptr_subgradient_packet.get()->theta=theta.value();
		ta_communicator->send();
// 		std::cout<<std::endl<<"size of sent packet: "<<sizeof(*ptr_subgradient_packet.get())<<std::endl<<std::endl;
		//std::cout<<"MANDO "<<((ptr_subgradient_packet.get()->busy)?("occupato"):("libero"))<<std::endl;
// 		std::cout<<"MANDO x:"<<x.value()<<" y:"<<y.value()<<std::endl;

	}

	if (selected_task=="") return "TASK_ASSIGNMENT_FAILED";
	else return selected_task;
	
}

double task_assignment::compute_alpha()
{	
        //così converge più in fretta, c'è da vedere se dal punto di vista teorico da problemi per convergere all'ottimo
        //la convergenza all'ottimo è garantita se i robot hanno alpha diversi?
        
	double alpha;
	
	double min=INF;
	
	for(int i=0;i<F.size();i++)
	{
	    if (min > F[i])
	    {
		min=F[i];
	    }
	}
	
	if((min/10)>1)
	{
	     alpha=-10;
	}
	else if(min>1)
	{
	     alpha=-1;
	}
	else alpha=-0.1;
	
	return alpha;
}
