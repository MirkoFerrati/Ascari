#include "task_assignment_simulator.h"

task_assignment_simulator::task_assignment_simulator()
{
    //written by Alessandro Settimi
    ta_router=0;
    ta_router_started=false;
    task_assignment_algorithm=-1;
    //written by Alessandro Settimi
}


bool task_assignment_simulator::initialize ( const Parsed_World& w )
{
    //written by Alessandro Settimi
    task_assignment_algorithm = w.task_assignment_algorithm;
    num_agents=w.agents.size();
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
