#ifndef FORMATION_CONTROL_AGENT_H
#define FORMATION_CONTROL_AGENT_H

#include "../HACK_KDEVELOP.h"
#ifdef ISAGENT

#include <vector>

#include <yaml_parser.h>
 
#include <random.hpp>
#include <types/events.h>

#include "../plugins/abstract_agent_plugin.h"
#include "../agent/agent.h"

class formation_control_agent: public abstract_agent_plugin
{
	
public:
	formation_control_agent(agent* a, Parsed_World* parse);
	
	void run_plugin();
	void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
	void compileExpressions(exprtk::symbol_table< double >& symbol_table);
	
	~formation_control_agent();

private:
	double v;
	double w;
	
	exprtk::expression<double> x;
	exprtk::expression<double> y;
	exprtk::expression<double> theta;
	std::string my_id;
	initial_state_value x0;
	initial_state_value y0;
};
	#endif

#endif // FORMATION_CONTROL_AGENT_H
