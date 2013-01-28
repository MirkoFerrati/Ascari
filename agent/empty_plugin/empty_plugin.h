#ifndef EMPTY_PLUGIN_H
#define EMPTY_PLUGIN_H

#include "plugin_module.h"
#include "communication/empty_plugin_communicator.hpp"
#include "typedefs.h"



class empty_plugin: public Plugin_module
{
public:
	empty_plugin( std::map< transition, events::value >& events,
				 const std::map<std::string, transition>& events_to_index, std::string identifier,
			  simulation_time& time);
void addReservedVariables(exprtk::symbol_table< double >& symbol_table_reference);
void compileExpressions(exprtk::symbol_table< double >& symbol_table_reference);
void run_plugin();
private:
//Some variables used in yaml file
double variable1;
double variable2, variable3;
//a variable taken from yaml file
double delta_variable;
//the expression used to calculate delta_variable
exprtk::expression<double> delta_expression;

//id of the agent
std::string identifier;

//communicator
empty_communicator_base * communicator;

//used by communicator
std::vector<empty_packet> data_receive;
empty_packet data_send;
std::shared_ptr<std::mutex> data_mutex;
};

#endif // EMPTY_PLUGIN_H
