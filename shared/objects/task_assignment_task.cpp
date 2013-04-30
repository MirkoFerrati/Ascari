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
  state.available=true;
  state.done=false;
  object_type="TA_task";
}

task_assignment_task::~task_assignment_task()
{
  state.time=0;
  state.executing=false;
  state.available=true;
  state.done=false;
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
	
	bool a=true;
	
	for ( std::map<std::string,agent_state_packet>::const_iterator it=agents.internal_map.begin(); it!=agents.internal_map.end(); ++it )
        {
		if(norm2(it->second.state.at(x),it->second.state.at(y),state.task_position[0],state.task_position[1]) < 0.2)
		{
			if(!state.executing)
			{
				state.time=time;
				state.executing=true;
				state.owner=it->first;
				state.available=false;
			}
			else
			{
				if (time-state.time>=state.task_execution_time) state.done=true;
			}
			a=false;
		}
	}
	
	if(a)
	{	
		state.executing=false;
		
		if (state.task_type==2 || state.task_type==0)
		{
			if(time - state.time >= state.task_execution_time + state.period)
			{
				state.available=true;
				state.done=false;
			}
		}
	}
	
	if (state.task_deadline>0 && time > state.task_deadline) state.available = false;
	
}