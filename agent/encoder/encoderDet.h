#ifndef ENCODERDET_H
#define ENCODERDET_H

#include "encoderAbstract.h"
#include "../topology/topology.h"
#include <typedefs.h>
#include <exprtk.hpp>
#include <vector>
#include <map>

class encoderDet:encoderAbstract
{
public:
	/**
	 * @param lambda reference alla locazione di memoria dove si trovano i valori dei lambda
	 * @param state reference alla locazione di memoria dove si trovano i valori dello stato
	 * @param bonusVariables reference alla locazione di memoria dove si trovano i valori delle variabili bonus
	 * @param stateVariablesName_to_Index mappa tra nomi delle variabili di stato e relativi indici
	 * @param bonusVariablesName_to_Index mappa tra nomi delle variabili bonus e relativi indici
	 * @param topology_expres mappa tra nomi delle topologie e relative espressioni
	 * @param topology_names nomi delle topologie
	 * @param lambda_expres mappa tra nomi delle lambda e relative espressioni
	 * @param lambda_names nomi delle lambda
	 */
		encoderDet(std::map< int, sub_event_value >& topologies, std::map< int, sub_event_value >& lambda,
			agent_state& state,agent_state& state_ith_agent,const std::map< std::string,int >& stateVariablesName_to_Index, 
			std::map<int,double> & bonusVariables,const std::map<std::string,int>& bonusVariablesName_to_Index,
			std::map<std::string,std::string>const& topology_expres, std::vector<std::string> const& topology_names,
			std::map<std::string,std::string>const& lambda_expres, std::vector<std::string> const& lambda_names
		);
		void computeSubEvents();
		
		
private:
	std::map<int,sub_event_value>& ref_lambda;
	topology topologies;
	index_map lambda_to_index;
	std::vector<exprtk::expression<double> > lambda_expressions;
	exprtk::symbol_table<double> symbol_table;
};

#endif // ENCODERDET_H
