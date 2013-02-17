//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

bool find_mininum_pair_task_id_cost(std::pair<task_id,task_cost> i,std::pair<task_id,task_cost> j)
{
      return i.second < j.second;
}

task_id task_assignment ::subgradient_algorithm()
{
	ptr_subgradient_packet.get()->agent_id=my_id;
      
	std::vector<subgradient_packet>& data_receive = *(std::vector<subgradient_packet>*)ta_communicator->get_data();
	      
	unsigned int w=0;
	
	unsigned int passi=1;
	
	update_costs_with_deadlines();
	printTaskCostMatrix(task_cost_matrix);
	  
	update_costs_with_position();
	printTaskCostMatrix(task_cost_matrix);
      
	double subgradient=0;
	
	double total_gradient=0;
	
	double new_value=0;
	
	std::map<agent_id,task_id> optimum_solution; //giusto per fare delle prove, il robot non la sa a prescindere
	
	/*optimum_solution.insert(std::make_pair("AGENTE1","T2"));
	optimum_solution.insert(std::make_pair("AGENTE2","T4"));
	optimum_solution.insert(std::make_pair("AGENTE3","T3"));
	optimum_solution.insert(std::make_pair("AGENTE4","T1"));*/ //ta_eg_9
	
	optimum_solution.insert(std::make_pair("AGENTE1","T2"));
	optimum_solution.insert(std::make_pair("AGENTE2","T7"));
	optimum_solution.insert(std::make_pair("AGENTE3","T4"));
	optimum_solution.insert(std::make_pair("AGENTE4","T3"));
	optimum_solution.insert(std::make_pair("AGENTE5","T1"));
	optimum_solution.insert(std::make_pair("AGENTE6","T6"));
	optimum_solution.insert(std::make_pair("AGENTE7","T5"));
	optimum_solution.insert(std::make_pair("AGENTE8","T10"));
	optimum_solution.insert(std::make_pair("AGENTE9","T8"));
	optimum_solution.insert(std::make_pair("AGENTE10","T9")); //ta_eg_10
	
	std::vector<task_id> fill;
		
	task_id my_task="";
	
	while(!converge && !s_interrupted)
	{
		std::cout<<"----------------PASSO "<<passi<<"----------------"<<std::endl<<std::endl;

		sleep(1);
		
		total_gradient = subgradient;
		
		
		ptr_receive_mutex->lock();
		
		fresh_data=false;
		
		for (unsigned int i=0;i<data_receive.size();i++)
		{
		    subgradient_task_packet temp = *(subgradient_task_packet*)data_receive.at(i).get_data();
		    agent_id name = data_receive.at(i).agent_id;
		    
		    if (!(name ==""))
		    {
			total_gradient += temp.subgradient;
		    }
		}
		
	        std::cout<<"G = "<<total_gradient<<std::endl<<std::endl;
		
		new_value=(total_gradient)/(agents_id.size());
				
		if (data_receive.size() == agents_id.size()-1)
		{
			for (unsigned int i=0;i<agents_id.size();i++)
			{			  
			      for (unsigned int j=0;j<tasks_id.size();j++)
			      {
				    if(/*COSA PRENDE NEW_VALUE?*/1)
				    {
					task_cost_matrix.at(agents_id.at(i)).at(tasks_id.at(j))=new_value;
				    }
			      }
			}
		}
		
		data_receive.clear();
		
		ptr_receive_mutex->unlock();
		
		
		printTaskCostMatrix(task_cost_matrix);
		
		copy_cost_matrix_to_cost_vector(task_cost_matrix);
		ta_problem.set_cost_vector(C);
		
		
		resolve_bilp_problem();
				
		//printTaskAssignmentMatrix();
		
		for (unsigned int i=0;i<tasks_id.size();i++)
		{
		      if (agent_task_assignment_vector->at(tasks_id.at(i))) my_task = tasks_id.at(i);
		}
		
		subgradient = task_cost_matrix.at(my_id).at(my_task);
		
		ptr_subgradient_packet.get()->data.subgradient=subgradient;
			
		
		std::cout<<my_id<<" invia: | g = "<<subgradient<<" : "<<my_task<<"|"<<std::endl;
		
		ta_communicator->send();
		
		std::cout<<" - "<<((my_task==optimum_solution.at(my_id))?("OPTIMUM"):("NOT OPTIMUM"))<<std::endl<<std::endl;
		
		convergence_control_routine(w-1);
		
		w=0;
		
		subgradient=0;
		
		total_gradient=0;
		
		passi++;
	}
		
	ta_communicator->send();

	for(unsigned int j=0;j<tasks_id.size();j++)
	{
		if(agent_task_assignment_vector->at(tasks_id[j]) == true) return tasks_id[j];
	}
	
	return "TASK_ASSIGNMENT_FAILED";
}