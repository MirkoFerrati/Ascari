#ifndef AGENT_H
#define AGENT_H

#include <list>
#include <vector>
#include <typedefs.h>
#include "automaton/automatonabstract.h"
#include "controller/controller.h"
#include "identifierModule/identifier_module.h"
#include "communication/agent_communicator_abstract.h"
#include "communication/world_communicator_abstract.h"
#include "../shared/yaml_parser.h"
#include "encoder/encoderAbstract.h"
#include "decoder.h"

class agent
{
public:
	agent(std::string name, bool isDummy, const std::vector< Parsed_Agent >& agents);
	~agent();
	
protected:
	/**
	 * name of agent and it is used for univocal indentification
	 */
	std::string identifier;
	
	/**
	 * The continuos state of the agent, used in the dynamic
	 */
	agent_state state;
	index_map map_statename_to_id;
	
	std::map<std::string, agent_state_packet> state_other_agents;
	
	
	std::map<int,double> bonusVariables;
	std::map<std::string,int> bonus_variables_to_Index;
	
	
	vector<automaton_state> discreteState;
	index_map map_discreteStateName_to_id;
	map<automaton_state, int> map_discreteStateId_to_controllerId;
	
	//in dummy agent, we will initialize a nautomaton, in agent an automaton
	automatonAbstract* automaton;
	
	encoderAbstract* encoder;
	
	/**
	 * Valori dei sottoeventi lambda e topology
	 */
	std::map<int,sub_event_value> sub_events; 
	
	/**
	 * Mappa dei nomi dei sottoeventi e dei relativi indici
	 */
	index_map sub_events_to_index;
	
	/**
	 * Valori degli eventi
	 */
	std::map<transition,bool> events;
	
	/**
	 * Mappa dei nomi degli eventi e dei relativi indici
	 */
	index_map events_to_index;
	
	/**
	 * The value of control variables, update by controllers
	 */
	control_command inputs;
	index_map map_inputs_name_to_id;
  
	/**
	 * every possible controller must be created and pushed inside this vector from the constructor
	 * and never modified again
	 */
	std::vector<controller> controllers;
	index_map map_controllername_to_id;
	
	decoder event_decoder;
	
	//in dummy we will not use this, since it is used for inter-agent communication
	//Note that we need two versions, one for net and one for shared memory
	agent_communicator_abstract* agent_comm;
	
	//in dummy this will be a way to access identifier informations, in agent this will communicate with simulator (tcp or shared memory)
	world_communicator_abstract* world_comm;
	
	//in dummy agent this will not be initialized
	identifier_module* idModule;
	
	void main_loop();
	
	/**
	 * crea la tabella di transizione dell'automaton
	 */
	transitionTable createAutomatonTableFromParsedAgent(const Parsed_Agent& agent);
	
	/**
	 * crea i sottoeventi e gli eventi per il decoder
	 */
	void createEventsFromParsedAgent(const Parsed_Agent& agent);
	
	/**
	 * crea i sottoeventi e l'encoder
	 */
	void createSubEventsFromParsedAgent(const Parsed_Agent& agent);
	
	void createDiscreteStateFromParsedAgent(const Parsed_Agent& agent);
	
	/**
	 * crea le strutture dati che rappresentano lo stato continuo, le variabili di controllo e lo stato discreto
	 */
	void createStateFromParsedAgent(const Parsed_Agent& agent);

};

#endif // AGENT_H
