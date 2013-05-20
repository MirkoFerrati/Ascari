#ifndef AGENT_H
#define AGENT_H


#include <list>
#include <vector>
#include <forward_list>
 
#include "automaton/automatonabstract.h"
#include "controller/controller.h"
#include "communication/agent_communicator_abstract.h"
#include "communication/world_communicator_abstract.h"
#include "../shared/yaml_parser.h"
#include "encoder/encoderAbstract.h"
#include "decoder/decoder.h"
#include "random.hpp"
#include "../plugins/abstract_agent_plugin.h"



class agent
{
public:
    agent (std::string name,  const std::unique_ptr< Parsed_Behavior >& behavior, const Parsed_World& world);
    agent ( const Parsed_World& world, bool noStart = false );
    ~agent();
    void start();
    void main_loop();
    void addPlugin( abstract_agent_plugin* plugin );
    /**
     * Setta il comunicatore
     */
    agent(const agent&)=delete;
    void setCommunicator (std::shared_ptr< agent_namespace::world_communicator_abstract >& communicator);

    /**
     * Setta l'identificatore
     */

    void setControlCommandIdentifier (std::string);


    inline std::forward_list<automaton_state> & getDiscreteStates() {
        return discreteState;
    }
    
    bool initialized;
    
    inline void setDiscreteState(int new_state) {
	discreteState.clear();
        discreteState.push_front(new_state);
    }

	
public:
    
	void initialize();

	
	/**
	 * Name of agent and it is used for unique indentification
	 */
	std::string identifier;
	
	/*! Simulation time */
	simulation_time time;
	
	/**
	 * The continuos state of the agent, used in the dynamic
	 */
	agent_state state;
	index_map map_statename_to_id;
	
	//std::map<std::string, agent_state_packet> state_other_agents;
	
	
	std::map<int,double> bonusVariables;
	std::map<std::string,int> map_bonus_variables_to_id;
	
	
	std::forward_list<automaton_state> discreteState;
	index_map map_discreteStateName_to_id;
	std::map<automaton_state, int> map_discreteStateId_to_controllerId;
	
	//in dummy agent, we will initialize a nautomaton, in agent an automaton
	automatonAbstract* automaton;
	
	encoderAbstract* encoder;
	
	/**
	 * Valori dei sottoeventi lambda e topology
	 */
	std::map<int,Sub_events> sub_events; 
	
	/**
	 * Mappa dei nomi dei sottoeventi e dei relativi indici
	 */
	index_map sub_events_to_index;
	
	/**
	 * Valori degli eventi
	 */
	std::map<transition,Events> events;
	
	/**
	 * Mappa dei nomi degli eventi e dei relativi indici
	 */
	std::map<std::string,transition> events_to_index;
	
	/**
	 * The value of control variables, update by controllers
	 */
	control_command_packet inputs;
	index_map map_inputs_name_to_id;
  
	/**
	 * every possible controller must be created and pushed inside this vector from the constructor
	 * and never modified again
	 */
	std::vector<controller> controllers;
	exprtk::symbol_table<double> symbol_table,encoder_symbol_table;
	index_map map_controllername_to_id;
	
	decoder event_decoder;
	
	    //std::map<std::string,task_assignment_task> objects;
	objects_container objects;

	//in dummy we will not use this, since it is used for inter-agent communication
	//Note that we need two versions, one for net and one for shared memory
	//Communications for now are only used by plugins, so agent_comm is useless here
	//agent_communicator_abstract* agent_comm;
	
	//in dummy this will be a way to access identifier informations, in agent this will communicate with simulator (tcp or shared memory)
	    std::shared_ptr<agent_namespace::world_communicator_abstract> world_comm;

	
	//in dummy agent this will not be initialized
	//The identifier_module has become a plugin, so idModule is useless here
	//identifier_module* idModule;
	
	/*!
	 * Moduli aggiuntivi utilizzabili stile plugin 
	 */
	std::vector <abstract_agent_plugin*> plugins;
	

	/**
	 * crea la tabella di transizione dell'automaton
	 */
	transitionTable createAutomatonTableFromParsedAgent(const std::unique_ptr<Parsed_Behavior>& behavior);
	
	/**
	 * crea i sottoeventi e gli eventi per il decoder
	 */
	void createEventsFromParsedAgent(const std::unique_ptr<Parsed_Behavior>& behavior);
	
	/**
	 * crea i sottoeventi e l'encoder
	 */
	void createSubEventsFromParsedAgent(const std::unique_ptr<Parsed_Behavior>& behavior);
	
	void createControllersFromParsedAgent(const std::unique_ptr<Parsed_Behavior>& behavior);
	
	void createDiscreteStateFromParsedAgent(const std::unique_ptr<Parsed_Behavior>& behavior);
	
	/**
	 * crea le strutture dati che rappresentano lo stato continuo, le variabili di controllo e lo stato discreto
	 */
	void createStateFromParsedAgent(const std::unique_ptr<Parsed_Behavior>& behavior);

    void createBonusVariablesFromWorld(std::map< bonusVariable, bonus_expression > bonus);
    

private:
      void init ( const std::unique_ptr< Parsed_Behavior >& behavior, bool noStart = false );

    rndom<double>* f_rndom;
    double pi;
    std::unique_ptr< Parsed_Behavior > const& behavior;
    Parsed_World const& world;
    bool noStart;
    bool isDummy;
};

#endif // AGENT_H
