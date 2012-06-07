#include "dynamic.h"
#include "typedefs.h"
#include "logog.hpp"
dynamic::dynamic(const agent_state& StateReferenceWARN, const control_command& controlReferenceWARN, std::map< std::string, std::string > expression, std::vector< std::string > variables_name)
{

}

agent_state dynamic::getNextState()
{
	ERR("not implemented");
	throw "not implemented";
}
