#include "task_assignment_simulator.h"
#include "task_assignment_parser_plugin.h"
#include <objects/task_assignment_task.h>
#include <types/world_sim_packet.h>
#include "task_assignment_plugin.h"

task_assignment_simulator::task_assignment_simulator(simulator* s):sim_packet(s->sim_packet)
{
    //written by Alessandro Settimi
    ta_router=0;
    ta_router_started=false;
    task_assignment_algorithm=-1;
    //written by Alessandro Settimi
}


bool task_assignment_simulator::initialize ( const Parsed_World& w )
{
    
    auto world=reinterpret_cast<task_assignment_parsed_world*>(w.parsed_items_from_plugins.at(TA_PLUGIN_IDENTIFIER));
    this->world=*world;

    task_assignment_algorithm = world->task_assignment_algorithm;
    num_agents=w.agents.size();
     if ( task_assignment_algorithm == SUBGRADIENT )
                {
                    ta_router = std::make_shared<task_assignment_router<task_assignment_namespace::subgradient_packet>> ( num_agents,world->agents );
                    ta_router_started=true;

                }
    return true;
}

void task_assignment_simulator::stop()
{
//TODO
}


std::list<abstract_object*>* task_assignment_simulator::create_objects()
{
    std::vector<std::string> tasks_id=world.tasks_id;
    
    std::cout<<"CREO OGGETTI - "<<tasks_id<<std::endl;
    
    std::list<abstract_object*>* list=new std::list<abstract_object*>;
    
    for ( unsigned int i=0; i<tasks_id.size();i++ )
    {
	abstract_object* temp;
	
	task_assignment_task* temp2 = new task_assignment_task();
      
	temp2->object_type="TASK_ASSIGNMENT";
	
	temp2->name=tasks_id.at(i);
	
	temp2->state=world.task_list.at(tasks_id.at(i));
	
	temp2->state.done=false;

// 	std::cout<<temp2<<std::endl;
	
 	temp=temp2;
	
// 	std::cout<<" - abstract - ";temp->print(std::cout);
	
	list->push_back(temp);
    }
    
    return list;
}


void task_assignment_simulator::run_plugin()
{
            //written by Alessandro Settimi



            if ( !ta_router_started )
            {
               

                /*if (task_assignment_algorithm == -1)
                {
                      //ERR("attenzione, algoritmo per il task assignment non selezionato");
                }*/
            }
            //written by Alessandro Settimi
}
