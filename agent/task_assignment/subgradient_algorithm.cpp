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
	printTaskCostMatrix();
	  
	update_costs_with_position();
	printTaskCostMatrix();
      
	double subgradient=0;
	
	double total_gradient=0;
	
	task_id free_task;
	
	std::map<agent_id,task_id> fixed_vector;
	
	task_assignment_namespace::task_cost_matrix forced_cost_matrix;
	
	task_cost_vector temp1;
    
	for(unsigned int i=0;i<agents_id.size();i++)
	{

	      for(unsigned int j=0;j<tasks_id.size();j++)
	      {
		  temp1.insert(std::make_pair(tasks_id.at(j),0));
	      }

	      forced_cost_matrix.insert(std::make_pair(agents_id.at(i),temp1));
	      
	      temp1.clear();
	}
	
	std::map<agent_id,task_id> control;
	
	std::vector<task_id> fill;
	
	bool test=false;
	
	for (unsigned int i=0;i<agents_id.size();i++)
	{
		control.insert(std::make_pair(agents_id.at(i),""));
		
		fixed_vector.insert(std::make_pair(agents_id.at(i),"")); //task a caso all'inizio
	}
     
	task_id& forced_task = fixed_vector.at(my_id);
	
	resolve_bilp_problem();
		
	printTaskAssignmentMatrix();
	
	printTaskCostMatrix();
	
	for (unsigned int i=0;i<tasks_id.size();i++)
	{
	      if (agent_task_assignment_vector->at(tasks_id.at(i)))
		  free_task = tasks_id.at(i);
	}
	
	std::cout<<"FREE TASK: "<<free_task<<std::endl<<std::endl;
	
	forced_cost_matrix.at(my_id)=*agent_task_cost_vector;
	
	while(!converge && !s_interrupted)
	{
		std::cout<<"----------------PASSO "<<passi<<"----------------"<<std::endl<<std::endl;

		sleep(1);
		    
		subgradient = forced_cost_matrix.at(my_id).at(free_task);

		if (!(forced_task=="") && forced_task!=free_task)
		{
		      subgradient += forced_cost_matrix.at(my_id).at(forced_task);
		      subgradient /= 2;
		}
		
		total_gradient = subgradient;
		
		ptr_receive_mutex->lock();
		
		fresh_data=false;
		
		for (unsigned int i=0;i<data_receive.size();i++)
		{
		    subgradient_task_packet temp = *(subgradient_task_packet*)data_receive.at(i).get_data();
		    agent_id name = data_receive.at(i).agent_id;
		    
		    if (!(name ==""))
		    {
			control.at(name)=temp.task;
			
			total_gradient += temp.subgradient;
		    }
		}

		ptr_receive_mutex->unlock();
				
		data_receive.clear();
			
		control.at(my_id)=forced_task;
		
		for (unsigned int i=0;i<agents_id.size();i++)
		{
		      for (unsigned int j=0;j<tasks_id.size();j++)
		      {
			    if (task_assignment_matrix.at(agents_id.at(i)).at(tasks_id.at(j)))
				if(forced_cost_matrix.at(agents_id.at(i)).at(tasks_id.at(j))==0)
				  forced_cost_matrix.at(agents_id.at(i)).at(tasks_id.at(j))=total_gradient;
		      }
		}
		
		
		task_cost_matrix = forced_cost_matrix;
		
		copy_cost_matrix_to_cost_vector();
		ta_problem.set_cost_vector(C);
		
		resolve_bilp_problem();
		
		for (unsigned int i=0;i<agents_id.size();i++)
		{
		      for (unsigned int j=0;j<tasks_id.size();j++)
		      {
			    if (task_assignment_matrix.at(agents_id.at(i)).at(tasks_id.at(j)))
				fixed_vector.at(agents_id.at(i))=tasks_id.at(j);
		      }
		}
			
		std::cout<<"FORCED ";printTaskCostMatrix();
		
		std::cout<<"FORCED TASK: "<<forced_task<<std::endl<<std::endl;
		
		std::cout<<"G = "<<total_gradient<<std::endl;
		
		
		ptr_subgradient_packet.get()->data.subgradient=subgradient;
		ptr_subgradient_packet.get()->data.task=forced_task;
			
		
		std::cout<<"Invio: | g = "<<subgradient<<":"<<forced_task<<"|"<<std::endl;
		
		ta_communicator->send();
			  
		
		std::cout<<"C=|";
		for (unsigned int i=0;i<agents_id.size();i++)
		{
		      task_id temp = control.at(agents_id.at(i));
		      
		      if (temp!="")
		      {
			  for (unsigned int j=0;j<fill.size();j++)
			      test = test || (fill.at(j)==temp);
			
			  if (!test) fill.push_back(temp);
		      }
		      test=false;
		      
		      std::cout<<agents_id.at(i)<<':'<<temp<<'|';
		}
		
		w=fill.size();
		std::cout<<' '<<w<<std::endl;
		
		convergence_control_routine(w-1);
			  
		
		w=0;
		
		fill.clear();
		

		
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