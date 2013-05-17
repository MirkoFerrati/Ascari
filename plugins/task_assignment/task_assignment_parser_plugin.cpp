#include "task_assignment_parser_plugin.h"
#include <logog.hpp>

  task_assignment_parser_plugin::task_assignment_parser_plugin()
  {
    enabled=false;
  }


abstract_parsed_world_plugin* task_assignment_parser_plugin::parseWorld ( const YAML::Node& node)
{
//   assert(wor==0);
   task_assignment_parsed_world* wo=new task_assignment_parsed_world();
//    task_assignment_parsed_world* wo=reinterpret_cast<task_assignment_parsed_world*> ( wor );
   wo->task_assignment_algorithm=-1;

      const YAML::Node &agent_nodes=node[0]["AGENTS"];
    wo->agents.reserve ( agent_nodes.size() );
     for ( unsigned int i=0; i<agent_nodes.size(); i++ )
    {
        std::string tmp_ag_name;
        agent_nodes[i]["AGENT"]>>tmp_ag_name;
      wo->agents.push_back(tmp_ag_name); 
    }
   
  if ( node[0].FindValue ( "WORLD" ) )
    {
        if ( node[0]["WORLD"].size() >0 )
        {

            if ( node[0]["WORLD"][0].FindValue ( "TASK_ASSIGNMENT_ALGORITHM" ) && node[0]["WORLD"][0].FindValue ( "TASK_NUMBER" ) && node[0]["WORLD"][0].FindValue ( "TASK_LIST" ) )
            {
	      enabled=true;
                unsigned int task_number;
                std::string algorithm;

                node[0]["WORLD"][0]["TASK_NUMBER"]>>task_number;

                node[0]["WORLD"][0]["TASK_ASSIGNMENT_ALGORITHM"]>>algorithm;

                if ( algorithm == "SUBGRADIENT" ) wo->task_assignment_algorithm = SUBGRADIENT;

                if ( wo->task_assignment_algorithm == -1 )
                {
                    ERR ( "UNDEFINED TASK ASSIGNMENT ALGORITHM",NULL );
                    return 0;
// 		      throw "UNDEFINED TASK ASSIGNMENT ALGORITHM";
                }

                createTaskList ( node[0]["WORLD"][0]["TASK_LIST"],wo->task_list,wo->tasks_id,task_number );
            }
        }
    }
    //return  std::dynamic_pointer_cast<abstract_parsed_world_plugin>(std::make_shared<task_assignment_parsed_world>(wo));
    return wo;
//     return enabled;
}

bool task_assignment_parser_plugin::parseAgent ( const YAML::Node& node,abstract_parsed_agent_plugin* ag )
{
  assert(ag==0);
  if (!enabled)
    return false;
   //written by Alessandro Settimi
    ag=new task_assignment_parsed_agent();
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

            reinterpret_cast<task_assignment_parsed_agent*> (ag)->agent_task_cost_vector.insert ( make_pair ( id,cost ) );
            i=i+2;
        }
            return true;

    }
    else
      return false;
    //written by Alessandro Settimi
    //return std::dynamic_pointer_cast<abstract_parsed_agent_plugin>(std::make_shared<task_assignment_parsed_agent>(ag));
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
