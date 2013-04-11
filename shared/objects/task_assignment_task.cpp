#include "task_assignment_task.h"
#include <math.h>
#include <iostream>
#include <sstream>


task_assignment_task::task_assignment_task()
{

}


task_assignment_task::task_assignment_task (task_assignment_namespace::task const& me )
{
  state=me;
  state.time=0;
  state.executing=false;
  object_type="TA_task";
}

task_assignment_task::~task_assignment_task()
{
  state.time=0;
  state.executing=false;
  object_type="TA_task";
}


void task_assignment_task::updateState ( const simulation_time& time, const agents_name_to_states& agents, const index_map& indexes )
{
		  int x=indexes.at("X");
		  int y=indexes.at("Y");
		  	std::stringstream app(std::stringstream::out);
	app.str("");
	
for ( std::map<std::string,agent_state_packet>::const_iterator it=agents.internal_map.begin(); it!=agents.internal_map.end(); ++it )
		{
		  if(app.str()=="")
			{
				if (std::sqrt( (it->second.state.at(x)-state.task_position[x])*(it->second.state.at(x)-state.task_position[x]) 
				  + (it->second.state.at(y)-state.task_position[y])*(it->second.state.at(y)-state.task_position[y]) ) < 0.15)
				{
					app << state.id << " (" << ((state.task_execution_time-(time-state.time) >= 0)?
					state.task_execution_time-(time-state.time):(0)) << ")";

					if(!state.executing)
					{
					  state.time=time;
					}
					state.executing=true;
				}
			}
		}
		if(app.str()=="")
		{
			state.executing=false;
			app << state.id << " (" << state.task_execution_time << ")";
		}
}

//     	template <typename Archive>
// 	void task_assignment_task::serialize(Archive& ar, const unsigned int /*version*/)
// 	{
// 		ar& object_type;
// 		ar& state;
// 	}

