//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

task_id task_assignment::cost_exchange_algorithm()
{ 
     ptr_cost_exchange_packet.get()->agent_id=my_id;
  
     std::vector<cost_exchange_packet>& data_receive = *(std::vector<cost_exchange_packet>*)ta_communicator->get_data();
          
     unsigned int w=0;
     
     unsigned int passi=1;
     
     update_costs_with_deadlines();
     printTaskCostMatrix(task_cost_matrix);
      
     update_costs_with_position();
     printTaskCostMatrix(task_cost_matrix);
          
     task_cost_vector basic_values;
     
     basic_values = *agent_task_cost_vector;
     
     while(!converge && !s_interrupted)
     {
	    std::cout<<"----------------PASSO "<<passi<<"----------------"<<std::endl<<std::endl;

	    sleep(1);
	    
	    resolve_bilp_problem();
	    
	    ptr_receive_mutex->lock();
	    
	    fresh_data=false;
	    
	    for (unsigned int i=0;i<data_receive.size();i++)
	    {
		solution_cost_packet temp = *(solution_cost_packet*)data_receive.at(i).get_data();
		agent_id name = data_receive.at(i).agent_id;
		
		if (!(name ==""))
		{
		    if (task_assignment_matrix == temp.ta_matrix) w++;
		    
		    if (!temp.costs.empty()) //controllo quali costi ricevo
		    {
			for (unsigned int j=0;j<tasks_id.size();j++)
			{
			     if (temp.costs.count(tasks_id.at(j)))
			     {    
				  task_cost_matrix.at(name).at(tasks_id.at(j))=temp.costs.at(tasks_id.at(j));
			     }
			}
		    }
		    
		    update_costs_with_expiring_deadlines(basic_values);
		    
		    for (unsigned int k=0;k<tasks_id.size();k++) //controllo quali costi devo inviare
		    {
			  if (temp.ta_matrix.at(my_id).at(tasks_id.at(k)) && !ptr_cost_exchange_packet.get()->data.costs.count(tasks_id.at(k)))
			  {
				  ptr_cost_exchange_packet.get()->data.costs.insert(std::make_pair(tasks_id.at(k),task_cost_matrix.at(my_id).at(tasks_id.at(k)))); 
			  }
		    }
		}
	    }
	    
	    copy_cost_matrix_to_cost_vector(task_cost_matrix);
	    ta_problem.set_cost_vector(C);
	    
	    data_receive.clear();
	    	    
	    ptr_cost_exchange_packet.get()->data.ta_matrix=task_assignment_matrix;
	    	    
	    std::cout<<"Invio: |";
	    for (task_cost_vector::iterator iter=ptr_cost_exchange_packet.get()->data.costs.begin();iter!=ptr_cost_exchange_packet.get()->data.costs.end();iter++)
		std::cout<<iter->first<<'-'<<iter->second<<'|';
	    std::cout<<std::endl;
	    
	    ta_communicator->send();
	    
	    ptr_cost_exchange_packet.get()->data.costs.clear();
	    
	    convergence_control_routine(w);
	    
	    w=0;
	    
	    ptr_receive_mutex->unlock();
	    
	    passi++;
     }
     
     ta_communicator->send();
     
     printTaskCostMatrix(task_cost_matrix);

     for(unsigned int j=0;j<tasks_id.size();j++)
     {
	    if(agent_task_assignment_vector->at(tasks_id[j]) == true) return tasks_id[j];
     }
     
     return "TASK_ASSIGNMENT_FAILED";
}