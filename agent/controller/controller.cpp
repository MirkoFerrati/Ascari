#include "controller.h"
#include <logog.hpp>

using namespace std;

void controller::computeControl()
{
	for (unsigned int i=0;i<expressions.size();i++)
	{
		expressions[i].value();
	}
}

controller::controller(const map< int,string >& map_id_expressions,
                       const vector< string >& controlVariablesName,
                       exprtk::symbol_table<double>& symbol_table
                      )
{
	
	
	
    exprtk::parser<double> parser;
	
	for (unsigned int i=0;i<controlVariablesName.size();i++)
	{
		exprtk::expression<double> expression;
		expression.register_symbol_table(symbol_table);
		string tmp_expression=controlVariablesName[i];
		tmp_expression.append(":=").append(map_id_expressions.at(i));
		if (parser.compile(tmp_expression,expression))
		{
			expressions.push_back(expression);
		}
		else
		{
			ERR("impossibile creare l'espressione: %s %s",tmp_expression.c_str(),parser.error().c_str());
// 			throw "impossibile creare l'espressione";
		}
	}
	
}
