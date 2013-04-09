//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

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

     std::cout<<"TASK DONE or ASSIGNED: "<<num_task-ut-1<<" CONTROL :"<<control<<std::endl; //TODO:-1 se c'è recharge
     
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
      
     a = a && (step>5);
	 
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
	    
		std::vector<subgradient_packet>& data_receive = *(std::vector<subgradient_packet>*)ta_communicator->get_data();
		
		std::cout<<"----------------PASSO "<<passi<<" ("<<time<<") ----------------"<<std::endl<<std::endl;

		
		for (unsigned int i=0;i<num_task;i++)
		{
		      F.at(i) = C.at(i) + D.at(i) + mu_T.at(i);
		      
		      if(tasks_id.at(i)!="RECHARGE" && (((!tasklist.at(tasks_id.at(i)).executing && !done_task.at(tasks_id.at(i)))) || my_task==tasks_id.at(i))) F.at(i) += ((remaining_times_to_deadline.at(tasks_id.at(i))>50)?0:(-DL.at(i)));
		}
		

		  
		ta_problem.set_cost_vector(F);

		
		
		ta_problem.solve(solution);
	
	      
		for(unsigned int j=0;j<num_task;j++)//copy_solution_to_TA_vector
		{
			agent_task_assignment_vector->at(tasks_id.at(j))=solution.at(j);
		}

		
		solution.clear();
		
		
		selected_task="";
		
		for (unsigned int i=0;i<num_task;i++)
		{
		      if (agent_task_assignment_vector->at(tasks_id.at(i)) > 0.5)
			  selected_task = tasks_id.at(i);
		      
		      subgradient.at(i) = -(agent_task_assignment_vector->at(tasks_id.at(i)));
		      total_subgradient.at(i) = subgradient.at(i) + e_i.at(i);
		}
		
		
		
		
		ptr_receive_mutex->lock();
		
		fresh_data=false;
		
		
		for (unsigned int i=0;i<data_receive.size();i++)
		{
		    subgradient_task_packet temp = *(subgradient_task_packet*)data_receive.at(i).get_data();
		    agent_id name = data_receive.at(i).agent_id;
		    
		    if (!(name ==""))
		    {
			
			  for (unsigned int i=0;i<num_task;i++)
			  {
				total_subgradient.at(i) +=  temp.subgradient.at(i);
			  }
			  
			  if (data_receive.at(i).taken_task!="")
			  {
				done_task.at(data_receive.at(i).taken_task)=true;
				agent_task_cost_vector->at(data_receive.at(i).taken_task)=INF;
				copy_cost_vector_to_C();
				std::cout<<"task "<<data_receive.at(i).taken_task<<" e' preso"<<std::endl;
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
		    }
		    

		}
		
		data_receive.clear();
		
		ptr_receive_mutex->unlock();
		
		std::cout<<"SIGMA = |"; 
		for (unsigned int i=0;i<num_task;i++)
		{
			std::cout<< total_subgradient.at(i) <<'|';
		}
		std::cout<<std::endl;
		
		double min=INF;
 		
		for (unsigned int i=0;i<num_task;i++)
 		{
		      min=std::min(min,F.at(i));
 		}
		
		//alpha= -( fabs(0.1*min)+0.01 );
		
		alpha=-0.1;
		
		for (unsigned int i=0;i<num_task;i++)
		{
		      mu_T.at(i) = mu_T.at(i) + alpha* total_subgradient.at(i);
		}
		
			
		ptr_subgradient_packet.get()->data.subgradient=subgradient;
			
		ta_communicator->send();
		
		//std::cout<<"MANDO "<<((ptr_subgradient_packet.get()->busy)?("occupato"):("libero"))<<std::endl;
		
		control_print();
		
		converge=convergence_control_routine();
		
		passi++;
	}

	if (selected_task=="") return "TASK_ASSIGNMENT_FAILED";
	else return selected_task;
	
}