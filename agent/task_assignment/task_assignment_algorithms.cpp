//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

void task_assignment::convergence_control_routine(unsigned int w)
{
     if (w==agents_id.size()-1)
     {
	    std::cout<<"Converge, ";
	    printTaskAssignmentMatrix();
	    printTaskCostMatrix();
	    converge=true;
     }
    
     if(!converge)
     {
	    printTaskAssignmentMatrix();
	    printTaskCostMatrix();
     }
}

void task_assignment::resolve_bilp_problem()
{
    std::vector<bool> solution;
      
    ta_problem.solve(solution);

    copy_solution_to_TA_matrix(solution);
}

task_id task_assignment::cost_exchange_algorithm()
{
     ptr_cost_exchange_packet.get()->agent_id=my_id;
  
     std::vector<cost_exchange_packet>& data_receive = *(std::vector<cost_exchange_packet>*)ta_communicator->get_data();
          
     unsigned int w=0;
     
     unsigned int passi=1;
          
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
		std::string name = data_receive.at(i).agent_id;
		
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
		    
		    update_costs_with_expiring_deadlines();
		    
		    for (unsigned int k=0;k<tasks_id.size();k++) //controllo quali costi devo inviare
		    {
			  if (temp.ta_matrix.at(my_id).at(tasks_id.at(k)) && !ptr_cost_exchange_packet.get()->data.costs.count(tasks_id.at(k)))
			  {
				  ptr_cost_exchange_packet.get()->data.costs.insert(make_pair(tasks_id.at(k),task_cost_matrix.at(my_id).at(tasks_id.at(k)))); 
			  }
		    }
		}
	    }
	    
	    copy_cost_matrix_to_cost_vector();
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

     for(unsigned int j=0;j<tasks_id.size();j++)
     {
	    if(agent_task_assignment_vector->at(tasks_id[j]) == true) return tasks_id[j];
     }
     
     return "TASK_ASSIGNMENT_FAILED";
}

 
task_id task_assignment ::subgradient_algorithm()
{
     ptr_subgradient_packet.get()->agent_id=my_id;
  
     std::vector<subgradient_packet>& data_receive = *(std::vector<subgradient_packet>*)ta_communicator->get_data();
     
     double g=0;
     
     unsigned int w=0;
     
     unsigned int passi=1;
          
     while(!converge && !s_interrupted)
     {
	    std::cout<<"----------------PASSO "<<passi<<"----------------"<<std::endl<<std::endl;

	    sleep(1);
	    
	    resolve_bilp_problem();
	    
	    ptr_receive_mutex->lock();
	    
	    fresh_data=false;
	    
	    for (unsigned int i=0;i<data_receive.size();i++)
	    {
		if (!(data_receive.at(i).agent_id ==""))
		{
		    double temp = *(double*)data_receive.at(i).get_data();
		    
		    if (g == temp) w++;
		}
	    }
	    
	    data_receive.clear();
	    
	    //TODO: g= . . .
	    	    
	    ptr_subgradient_packet.get()->data=g;
	    
	    ta_communicator->send();
	    
	    convergence_control_routine(w);
	    
	    w=0;
	    
	    ptr_receive_mutex->unlock();
	    
	    passi++;
     }
     
     ta_communicator->send();

     for(unsigned int j=0;j<tasks_id.size();j++)
     {
	    if(agent_task_assignment_vector->at(tasks_id[j]) == true) return tasks_id[j];
     }
     
     return "TASK_ASSIGNMENT_FAILED";
}



task_id task_assignment ::solution_exchange_algorithm()
{    
     ptr_solution_exchange_packet.get()->agent_id=my_id;
  
     std::vector<solution_exchange_packet>& data_receive = *(std::vector<solution_exchange_packet>*)ta_communicator->get_data();
     
     unsigned int w=0;
     
     unsigned int passi=1;
     
     while(!converge && !s_interrupted)
     {
	    std::cout<<"----------------PASSO "<<passi<<"----------------"<<std::endl<<std::endl;

	    sleep(1);
	    
	    resolve_bilp_problem();
	    
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
	    	    
	    ptr_solution_exchange_packet.get()->data=task_assignment_matrix;
	    
	    ta_communicator->send();
	    
	    convergence_control_routine(w);
	    
	    w=0;
	    
	    ptr_receive_mutex->unlock();
	    
	    passi++;
     }
     
     ta_communicator->send();

     for(unsigned int j=0;j<tasks_id.size();j++)
     {
	    if(agent_task_assignment_vector->at(tasks_id[j]) == true) return tasks_id[j];
     }
     
     return "TASK_ASSIGNMENT_FAILED";
}

