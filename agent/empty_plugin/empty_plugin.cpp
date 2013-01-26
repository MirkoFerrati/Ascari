#include "empty_plugin.h"
#include "typedefs.h"
#include "logog.hpp"
empty_plugin::empty_plugin(std::map< transition, events::value >& events, 
						   const std::map< std::string, transition >& events_to_index, 
						   std::string identifier, simulation_time& time)
{
	//events gives you access to agent events, identifier is the agent name, time is the time of the simulation
}



void empty_plugin::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
	symbol_table.add_variable("VARIABLE1",variable1,false);
	symbol_table.add_variable("VARIABLE2",variable2,false);
	symbol_table.add_variable("VARIABLE3",variable3,false);
}


void empty_plugin::compileExpressions(exprtk::symbol_table< double >& symbol_table)
{
	exprtk::parser<double> parser;
	
	delta_expression.register_symbol_table(symbol_table);
	if (!parser.compile("x-y",delta_expression))
	{
		ERR("impossibile creare l'espressione: %s","x-y");
		throw "impossibile creare l'espressione";
	}
}



void empty_plugin::run_plugin()
{
	/**
	 * This code will run during every clock cycle of the main controller loop of the agent
	 **/
	
	/*
	if(!started)
	{
		do_something_to_start();
		started=true;
	}	
	if(stopped)
		return;
	do_the_plugin_stuff_here;
	//read variables in this way:
	delta_variable=delta_expression.value();
	//set variables in this way
	variable1=5;
	variable2=delta-variable-variable1;
	*/
}
