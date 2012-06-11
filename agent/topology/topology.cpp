#include "topology.h"
#include "logog.hpp"

using namespace std;

topology::topology(std::map< int, sub_event_value >& topologies, agent_state& state, agent_state& state_ith_agent,const std::map< string, 
		   int >& stateVariablesName_to_Index, std::map< int, double >& bonusVariables, const std::map< string,
		   int >& bonusVariablesName_to_Index, const map< string, string >& topology_expres, 
		   const vector< string >& topology_names)
:ref_topology(topologies),ref_state_ith_agent(state_ith_agent)
{
	
	for (map<string,int>::const_iterator it=stateVariablesName_to_Index.begin();it!=stateVariablesName_to_Index.end();++it)
	{
		symbol_table.add_variable(it->first,state.at(it->second));
	}
	
	//metto lo stato degli altri nello stesso ordine dello stato ma aggiungendo i in coda al nome
	for (map<string,int>::const_iterator it=stateVariablesName_to_Index.begin();it!=stateVariablesName_to_Index.end();++it)
	{	
		symbol_table.add_variable((it->first)+"i",state_ith_agent.at(it->second));
	}
	
	
	for (map<string,int>::const_iterator it=bonusVariablesName_to_Index.begin();it!=bonusVariablesName_to_Index.end();++it)
	{
		symbol_table.add_variable(it->first,bonusVariables.at(it->second));
	}
	symbol_table.add_constants();
	
	 exprtk::parser<double> parser;
	
	for (unsigned int i=0;i<topology_names.size();i++)
	{
		exprtk::expression<double> tmp_expression;
		tmp_expression.register_symbol_table(symbol_table);
		string string_tmp_expression=topology_expres.at(topology_names.at(i));
		parser.compile(string_tmp_expression,tmp_expression);
		topology_expressions.push_back(tmp_expression);
		topology_to_index.insert(make_pair(topology_names.at(i),i));
	}
	
}

void topology::computeTopologies()
{
	for (map<string,int>::const_iterator it=topology_to_index.begin();it!=topology_to_index.end();it++)
	{
		if (topology_expressions.at(it->second).value()==1)
			ref_topology[it->second]=_TRUE;
		if (topology_expressions.at(it->second).value()==0)
			ref_topology[it->second]=_FALSE;
	}
	
}


