#include "task_assignment_parser_plugin.h"
#include "yaml_parser.h"
#include <yaml-cpp/node/node.h>
#include <logog.hpp>

  task_assignment_parser_plugin::task_assignment_parser_plugin(std::string type):type(type)
  {
    enabled=false;
  }

bool task_assignment_parser_plugin::isEnabled()
{
      return enabled;
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
        std::string tmp_ag_name = agent_nodes[i]["AGENT"].as<std::string>();
      wo->agents.push_back(tmp_ag_name); 
    }
   
  if ( node[0]["WORLD"] )
    {
        if ( node[0]["WORLD"].size() >0 )
        {

            if ( node[0]["WORLD"][0]["TASK_ASSIGNMENT_ALGORITHM"] && node[0]["WORLD"][0]["TASK_NUMBER"] && node[0]["WORLD"][0]["TASK_LIST"])
            {
	      enabled=true;
                unsigned int task_number = node[0]["WORLD"][0]["TASK_NUMBER"].as<unsigned int>();
                std::string algorithm = node[0]["WORLD"][0]["TASK_ASSIGNMENT_ALGORITHM"].as<std::string>();

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
    return enabled?wo:0;
//     return enabled;
}

abstract_parsed_agent_plugin* task_assignment_parser_plugin::parseAgent ( const YAML::Node& node)
{
//   assert(ag==0);
  if (!enabled)
    return 0;
   //written by Alessandro Settimi
    abstract_parsed_agent_plugin* ag=new task_assignment_parsed_agent();
    if ( node["TASK_COST_VECTOR"] )
    {
        const YAML::Node& co=node["TASK_COST_VECTOR"];
        task_assignment_namespace::task_cost_vector app;
        double cost;
        std::string id;
        std::string temp;

        for ( unsigned int i=0; i<co.size(); )
        {
	    id = co[i].as<std::string>();
	    temp = co[i+1].as<std::string>();

            if ( temp=="INF" ) cost=INF;
            else cost = co[i+1].as<double>();

            reinterpret_cast<task_assignment_parsed_agent*> (ag)->agent_task_cost_vector.insert ( make_pair ( id,cost ) );
            i=i+2;
        }

    
    
	if( node["HOME"] )
	{
	    const YAML::Node& h=node["HOME"];
	    reinterpret_cast<task_assignment_parsed_agent*> (ag)->home_x = h[0].as<double>();
	    reinterpret_cast<task_assignment_parsed_agent*> (ag)->home_y = h[1].as<double>();
	}
    
	return ag;
      
    }
    
    else return 0;
}

//written by Alessandro Settimi
void task_assignment_parser_plugin::createTaskList ( const YAML::Node& node, task_assignment_namespace::task_list& task_list,std::vector<task_assignment_namespace::task_id>& tasks_id,
                      unsigned int task_number )
{
    unsigned int j=0;

    for ( unsigned int i=0; i<task_number*8; )
    {
        task_assignment_namespace::task_id temp1 = node[i].as<std::string>();
        tasks_id.push_back ( temp1 );

        task_assignment_namespace::task temp2;

	temp2.id = node[i].as<std::string>();
	temp2.task_position[0] = node[i+1].as<double>();
	temp2.task_position[1] = node[i+2].as<double>();
	temp2.task_position[2] = node[i+3].as<double>();
	temp2.task_type = node[i+4].as<int>();
	temp2.task_execution_time = node[i+5].as<double>();
	temp2.period = node[i+6].as<double>();
	temp2.task_deadline = node[i+7].as<double>();
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
