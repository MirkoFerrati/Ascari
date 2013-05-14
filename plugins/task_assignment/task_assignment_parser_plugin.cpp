#include "task_assignment_parser_plugin.h"
#include <logog.hpp>

  task_assignment_parser_plugin::task_assignment_parser_plugin()
  {
    
  }


std::shared_ptr< abstract_parsed_world_plugin > task_assignment_parser_plugin::parseWorld ( const YAML::Node& node )
{
      wo.task_assignment_algorithm=-1;

  if ( node[0].FindValue ( "WORLD" ) )
    {
        if ( node[0]["WORLD"].size() >0 )
        {

            if ( node[0]["WORLD"][0].FindValue ( "TASK_ASSIGNMENT_ALGORITHM" ) && node[0]["WORLD"][0].FindValue ( "TASK_NUMBER" ) && node[0]["WORLD"][0].FindValue ( "TASK_LIST" ) )
            {
                unsigned int task_number;
                std::string algorithm;

                node[0]["WORLD"][0]["TASK_NUMBER"]>>task_number;

                node[0]["WORLD"][0]["TASK_ASSIGNMENT_ALGORITHM"]>>algorithm;

                if ( algorithm == "SUBGRADIENT" ) wo.task_assignment_algorithm = SUBGRADIENT;

                if ( wo.task_assignment_algorithm == -1 )
                {
                    ERR ( "UNDEFINED TASK ASSIGNMENT ALGORITHM",NULL );
                    return false;
// 		      throw "UNDEFINED TASK ASSIGNMENT ALGORITHM";
                }

                createTaskList ( node[0]["WORLD"][0]["TASK_LIST"],wo.task_list,wo.tasks_id,task_number );
            }
        }
    }
}

std::shared_ptr< abstract_parsed_agent_plugin > task_assignment_parser_plugin::parseAgent ( const YAML::Node& node )
{
   //written by Alessandro Settimi

    if ( node.FindValue ( "TASK_COST_VECTOR" ) )
    {
        const YAML::Node& co=node["TASK_COST_VECTOR"];
        task_assignment_namespace::task_cost_vector app;
        double cost;
        std::string id;
        std::string temp;

        for ( unsigned int i=0; i<co.size(); )
        {
            co[i] >> id;
            co[i+1] >> temp;
            if ( temp=="INF" ) cost=INF;
            else co[i+1] >> cost;

            ag.agent_task_cost_vector.insert ( make_pair ( id,cost ) );
            i=i+2;
        }
    }
    //written by Alessandro Settimi
}

//written by Alessandro Settimi
void task_assignment_parser_plugin::createTaskList ( const YAML::Node& node, task_assignment_namespace::task_list& task_list,std::vector<task_assignment_namespace::task_id>& tasks_id,
                      unsigned int task_number )
{
    unsigned int j=0;

    for ( unsigned int i=0; i<task_number*8; )
    {
        task_assignment_namespace::task_id temp1;
        node[i] >> temp1;
        tasks_id.push_back ( temp1 );

        task_assignment_namespace::task temp2;

        node[i] >> temp2.id;
        node[i+1] >> temp2.task_position[0];
        node[i+2] >> temp2.task_position[1];
        node[i+3] >> temp2.task_position[2];
        node[i+4] >> temp2.task_type;
        node[i+5] >> temp2.task_execution_time;
        node[i+6] >> temp2.period;
        node[i+7] >> temp2.task_deadline;

        temp2.executing=false;
        temp2.owner="";
        temp2.time=0;
        temp2.available=true;

        task_list.insert ( make_pair ( temp1,temp2 ) );

        i=i+8;
        j++;
    }
}
//written by Alessandro Settimi
