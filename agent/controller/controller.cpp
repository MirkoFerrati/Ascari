#include "controller.h"

using namespace std;

void controller::computeControl()
{
	for (int i=0;i<expressions.size();i++)
	{
		expressions[i].value();
	}
}

controller::controller(agent_state& state_reference_WARN,
                       const vector<string >& stateVariablesName,
                       const map< int,string >& map_id_expressions,
                       const vector< string >& controlVariablesName,
                       control_command& control_reference_WARN
                      )
{
	exprtk::symbol_table<double> symbol_table;
    
	for (int i=0;i<state_reference_WARN.size();i++)
	{
		symbol_table.add_variable(stateVariablesName[i],state_reference_WARN[i]);
	}
	for (int i=0;i<control_reference_WARN.size();i++)
	{
		symbol_table.add_variable(controlVariablesName[i],control_reference_WARN[i]);
	}
	symbol_table.add_constants();
	
    exprtk::parser<double> parser;
	
	for (int i=0;i<controlVariablesName.size();i++)
	{
		exprtk::expression<double> expression;
		expression.register_symbol_table(symbol_table);
		string tmp_expression=controlVariablesName[i];
		tmp_expression.append(":=").append(map_id_expressions.at(i));
		parser.compile(tmp_expression,expression);
		expressions.push_back(expression);
	}
	
	
	
}
