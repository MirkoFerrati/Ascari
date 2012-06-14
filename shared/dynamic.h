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
	dynamic(agent_state& StateReferenceWARN, control_command& controlReferenceWARN, 
		std::map< std::string, std::string > expression, std::vector< std::string > state_variables_name, 
	 std::vector< std::string > control_variables_name);
	agent_state getNextState();
	 ~dynamic();
	
	
private:
	std::vector<exprtk::expression<double> > expressions;
	exprtk::symbol_table<double> symbol_table;
	std::map<std::string,int> map_expressions;
	const agent_state& StateReferenceWARN;
	
};

#endif // DYNAMIC_H
