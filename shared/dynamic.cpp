#include "dynamic.h"
#include "typedefs.h"
dynamic::dynamic()
{

}

agent_state dynamic::getNextState(const agent_state& oldState, const control_command& control)
{
	agent_state temp(oldState);
	for (map_int_double::const_iterator it=control.control_map.begin();it!=control.control_map.end();it++)
	{
		temp[it->first]+=it->second/10; //TODO: this is just a test line, nothing real: a=f(a,u) where f(a,u)=a+u 
	}
	return temp;
}
