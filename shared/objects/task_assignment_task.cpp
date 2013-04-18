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

double norm2(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

void task_assignment_task::updateState ( const simulation_time& time, const agents_name_to_states& agents, const index_map& indexes )
{
	int x=indexes.at("X");
	int y=indexes.at("Y");

	
	for ( std::map<std::string,agent_state_packet>::const_iterator it=agents.internal_map.begin(); it!=agents.internal_map.end(); ++it )
        {
		if(norm2(it->second.state.at(x),it->second.state.at(y),state.task_position[0],state.task_position[1]) < 0.25)
		{
			state.executing=true;
			state.owner=it->first;
			state.time=time;
		}	  
	}
}

//     	template <typename Archive>
// 	void task_assignment_task::serialize(Archive& ar, const unsigned int /*version*/)
// 	{
// 		ar& object_type;
// 		ar& state;
// 	}