#ifndef ENCODERDET_H
#define ENCODERDET_H

#include "encoderAbstract.h"
#include <exprtk.hpp>
#include <types/events.h>
#include <types.h>
#include <vector>
#include <map>

class encoderDet:public encoderAbstract
{
public:
	/**
	 * @param lambda reference alla locazione di memoria dove si trovano i valori dei lambda OUTPUT
	 * @param state reference alla locazione di memoria dove si trovano i valori dello stato INPUT 
	 * @param state_other_agents reference alla locazione di memoria dove verranno messi i valori dello stato dei vicini INPUT
	 * @param bonusVariables reference alla locazione di memoria dove si trovano i valori delle variabili bonus INPUT
	 * @param stateVariablesName_to_Index mappa tra nomi delle variabili di stato e relativi indici INPUT Constructor
	 * @param bonusVariablesName_to_Index mappa tra nomi delle variabili bonus e relativi indici INPUT Constructor
	 * @param topology_expres mappa tra nomi delle topologie e relative espressioni INPUT Constructor
	 * @param lambda_expres mappa tra nomi delle lambda e relative espressioni INPUT Constructor
	 * @param sub_events_to_index mappa tra gli indici delle topologie dei lambda e il relativo valore in sub_events
	 */
		encoderDet(std::map< int, Sub_events >& sub_events, const std::string & agent_name,
			agent_state& state,const std::map< std::string,int >& stateVariablesName_to_Index, 
			std::map<int,double> & bonusVariables,const std::map<std::string,int>& bonusVariablesName_to_Index,
			std::map<std::string,std::string>const& topology_expres,index_map const& sub_events_to_index,
			std::map<std::string,std::string>const& lambda_expres,exprtk::symbol_table<double>& symbol_table);
		void computeSubEvents(const std::map<std::string, agent_state_packet>& state_other_agents);
// 		~encoderDet();
		
private:
	std::map<int,Sub_events>& ref_sub_events; //lambda+topologies
	agent_state state_target;
	std::vector<const agent_state_packet*> ref_other_agents;
	std::map<int,exprtk::expression<double> > lambda_expressions;
	std::map<int,exprtk::expression<double> > topology_expressions;
	exprtk::symbol_table<double>& symbol_table;
	std::string agent_name;
};

#endif // ENCODERDET_H
