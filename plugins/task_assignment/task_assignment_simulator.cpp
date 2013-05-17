#include "task_assignment_simulator.h"
#include "task_assignment_parser_plugin.h"
#include <objects/task_assignment_task.h>
#include <types/world_sim_packet.h>

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
    auto world=reinterpret_cast<task_assignment_parsed_world*>(w.parsed_items_from_plugins[0]);
  //written by Alessandro Settimi
    task_assignment_algorithm = world->task_assignment_algorithm;
    num_agents=w.agents.size();
    
    auto tasklist=world->task_list;
    auto tasks_id=world->tasks_id;

    for ( unsigned int i=0; i<tasks_id.size(); i++ )
    {
        task_assignment_task tmp ( world->task_list.at ( tasks_id.at ( i ) ) );
        sim_packet.objects[tasks_id.at ( i )]=tmp;
    }

    std::cout<<sim_packet.objects<<std::endl;
    
    return true;
    //written by Alessandro Settimi
}

void task_assignment_simulator::stop()
{
//TODO
}


void task_assignment_simulator::run_plugin()
{
            //written by Alessandro Settimi



            if ( !ta_router_started )
            {
                if ( task_assignment_algorithm == SUBGRADIENT )
                {
                    ta_router = std::make_shared<task_assignment_router<task_assignment_namespace::subgradient_packet>> ( num_agents );
                    ta_router_started=true;

                }

                /*if (task_assignment_algorithm == -1)
                {
                      //ERR("attenzione, algoritmo per il task assignment non selezionato");
                }*/
            }
            //written by Alessandro Settimi
}
