#include "formation_control_agent.h"


void formation_control_agent::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
	symbol_table.add_variable("V_P",v,false);
	symbol_table.add_variable("W_P",w,false);
}


void formation_control_agent::compileExpressions(exprtk::symbol_table< double >& symbol_table)
{
	exprtk::parser<double> parser;
	
	x.register_symbol_table(symbol_table);
	if (!parser.compile("X",x))
	{
		ERR("impossibile creare l'espressione: %s","X");
		throw "impossibile creare l'espressione";
	}
	
	y.register_symbol_table(symbol_table);
	if (!parser.compile("Y",y))
	{
		ERR("impossibile creare l'espressione: %s","Y");
		throw "impossibile creare l'espressione";
	}
	
	theta.register_symbol_table(symbol_table);
	if (!parser.compile("THETA",theta))
	{
		ERR("impossibile creare l'espressione: %s","THETA");
		throw "impossibile creare l'espressione";
	}
}

 
formation_control_agent::~formation_control_agent()
{

}

