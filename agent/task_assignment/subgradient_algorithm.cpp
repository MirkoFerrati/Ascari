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
     
     if (num_robot < num_task) control=num_robot;
     else control=num_task;
     
     for (unsigned int i=0;i<num_task;i++)
     {
	    if (fabs(total_subgradient.at(i)) < 0.0001) iter++;
     }

     if(iter==control)
     {
	  step++;
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
	if(!converge)
	{
		ptr_subgradient_packet.get()->agent_id=my_id;
	    
		std::vector<subgradient_packet>& data_receive = *(std::vector<subgradient_packet>*)ta_communicator->get_data();
	      
		assignment_vector.clear();
		
		std::cout<<"----------------PASSO "<<passi<<"----------------"<<std::endl<<std::endl;

		sleep(1);
		
		update_distance_vector();
		
		for (unsigned int i=0;i<num_task;i++)
		{
		      F.at(i) = C.at(i) + D.at(i) + mu_T.at(i);
		}
		
		ta_problem.set_cost_vector(F);

		resolve_assignment_problem();
		
		selected_task="";
		
		for (unsigned int i=0;i<num_task;i++)
		{
		      if (agent_task_assignment_vector->at(tasks_id.at(i)) > 0.5)
			  selected_task = tasks_id.at(i);
		      
		      subgradient.at(i) = -(agent_task_assignment_vector->at(tasks_id.at(i)));
		      total_subgradient.at(i) = subgradient.at(i) + e_i.at(i);
		}
		
		assignment_vector.clear();
		
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
		      
		alpha= -1;
		
		for (unsigned int i=0;i<num_task;i++)
		{
		      mu_T.at(i) = mu_T.at(i) + alpha* total_subgradient.at(i);
		}
		
			
		ptr_subgradient_packet.get()->data.subgradient=subgradient;
			
		ta_communicator->send();
		
		control_print();
		
		converge=convergence_control_routine();
		
		for( unsigned int i=0;i<num_task;i++)
		{
		    subgradient.at(i)=0;
		    total_subgradient.at(i)=0;
		}
		
		passi++;
	}

	if (selected_task=="") return "TASK_ASSIGNMENT_FAILED";
	else return selected_task;
	
}