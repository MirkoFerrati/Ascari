#include "dynamic.h"
#include "typedefs.h"
#include "logog.hpp"

using namespace std;

dynamic::dynamic(agent_state& StateReferenceWARN,control_command& controlReferenceWARN, 
		 std::map< std::string, std::string > expression_map, std::vector< std::string > state_variables_name, 
		 std::vector< std::string > control_variables_name):StateReferenceWARN(StateReferenceWARN)
{
	//symbol_table=new exprtk::symbol_table<double>();
	for (unsigned int i=0;i<StateReferenceWARN.size();i++)
	{
		symbol_table.add_variable(state_variables_name.at(i),StateReferenceWARN.at(i));
	}
	for (unsigned int i=0;i<controlReferenceWARN.size();i++)
	{
		symbol_table.add_variable(control_variables_name.at(i),controlReferenceWARN.at(i));
	}
	symbol_table.add_constants();
	
    exprtk::parser<double> parser;
	
			
	
	
	for (unsigned int i=0;i<state_variables_name.size();i++)
	{
		exprtk::expression<double> expression_tmp;//=new exprtk::expression<double>();
		expression_tmp.register_symbol_table(symbol_table);
		string string_tmp_expression=expression_map.at(state_variables_name.at(i));
		if (!parser.compile(string_tmp_expression,expression_tmp))
		{
		  ERR("impossibile creare l'espressione: %s %s",string_tmp_expression.c_str(),parser.error().c_str());
// 		  throw "impossibile creare l'espressione";
		}
		expressions.emplace_back(expression_tmp);
		map_expressions.insert(make_pair(state_variables_name.at(i),i));
		
		
		  
	}
	
}

dynamic::~dynamic()
{
	//delete(symbol_table);
}


agent_state dynamic::getNextState()
{
  agent_state tmp;
  for (map<string,int>::const_iterator iter=map_expressions.begin(); iter!=map_expressions.end();++iter)
  {
    tmp.insert(make_pair(iter->second,StateReferenceWARN.at(iter->second)+T_CAMP*expressions.at(iter->second).value()));
  }
  return tmp;
}
