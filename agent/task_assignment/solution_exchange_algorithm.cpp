//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>
#include "typedefs.h"

using namespace task_assignment_namespace;

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