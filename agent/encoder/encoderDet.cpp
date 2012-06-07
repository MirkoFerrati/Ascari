#include "encoderDet.h"

using namespace std;

encoderDet::encoderDet(
	std::map< int, sub_event_value >& sub_events, 
	 agent_state& state, const std::map< string, int >& stateVariablesName_to_Index, 
	 std::map< int, double >& bonusVariables, const std::map< string, int >& bonusVariablesName_to_Index, 
	const std::map< string, string >& topology_expres, const std::vector< string >& topology_names, 
	const std::map< string, string >& lambda_expres, const std::vector< string >& lambda_names)
:ref_sub_events(sub_events)
{
	for (map<string,int>::const_iterator it=stateVariablesName_to_Index.begin();it!=stateVariablesName_to_Index.end();++it)
	{
		symbol_table.add_variable(it->first,state.at(it->second));
	}
	
	for (map<string,int>::const_iterator it=bonusVariablesName_to_Index.begin();it!=bonusVariablesName_to_Index.end();++it)
	{
		symbol_table.add_variable(it->first,bonusVariables.at(it->second));
	}
	symbol_table.add_constants();
	
	 exprtk::parser<double> parser;
	
	for (unsigned int i=0;i<lambda_names.size();i++)
	{
		exprtk::expression<double> tmp_expression;
		tmp_expression.register_symbol_table(symbol_table);
		string string_tmp_expression=lambda_expres.at(lambda_names.at(i));
		parser.compile(string_tmp_expression,tmp_expression);
		lambda_expressions.push_back(tmp_expression);
		lambda_to_index.insert(make_pair(lambda_names.at(i),i));
	}
	
}


void encoderDet::computeSubEvents()
{
	for (map<string,int>::const_iterator it=lambda_to_index.begin();it!=lambda_to_index.end();it++)
	{
		if (lambda_expressions.at(it->second).value()==1)
			ref_sub_events[it->second]=_TRUE;
		if (lambda_expressions.at(it->second).value()==0)
			ref_sub_events[it->second]=_FALSE;
	}
}
