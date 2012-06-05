#ifndef DYNAMIC_H
#define DYNAMIC_H
#include "typedefs.h"

#include "exprtk.hpp"

#include <vector>
#include <map>
#include <string>

class dynamic
{
public:
	dynamic(const agent_state& StateReferenceWARN, const control_command& controlReferenceWARN,
			std::map<std::string,std::string> expression,
			std::vector<std::string> variables_name
	);
	agent_state getNextState();
	
	
private:
	std::vector<exprtk::expression<double> > expressions;
};

#endif // DYNAMIC_H
