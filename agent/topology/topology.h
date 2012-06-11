#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <typedefs.h>
#include <exprtk.hpp>
#include <vector>
#include <map>


class topology
{
private:
  index_map topology_to_index;
  agent_state& ref_state_ith_agent; //nello stesso ordine dello stato
   
  std::map<int,sub_event_value>& ref_topology;	
  exprtk::symbol_table<double> symbol_table;
  std::vector<exprtk::expression<double> > topology_expressions;

public:
	/**
	 * @param sub_events reference alla locazione di memoria dove si trovano i valori dei sottoeventi
	 * @param state reference alla locazione di memoria dove si trovano i valori dello stato
	 * @param state_ith_agent reference alla locazione di memoria dove verranno messi i valori dello stato dei vicini
	 * @param bonusVariables reference alla locazione di memoria dove si trovano i valori delle variabili bonus
	 * @param stateVariablesName_to_Index mappa tra nomi delle variabili di stato e relativi indici
	 * @param bonusVariablesName_to_Index mappa tra nomi delle variabili bonus e relativi indici
	 * @param topology_expres mappa tra nomi delle topologie e relative espressioni
	 * @param topology_names nomi delle topologie
	 */
		topology(std::map< int, sub_event_value >& topologies,
			agent_state& state, agent_state& state_ith_agent,const std::map< std::string,int >& stateVariablesName_to_Index, 
			std::map<int,double> & bonusVariables,const std::map<std::string,int>& bonusVariablesName_to_Index,
			std::map<std::string,std::string>const& topology_expres, std::vector<std::string> const& topology_names);
		void computeTopologies();
  std::map< std::string, int >::const_iterator lambda_to_index();
		
};

#endif // TOPOLOGY_H