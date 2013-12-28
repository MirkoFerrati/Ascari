#ifndef AGENT_H
#define AGENT_H

#include "../plugins/HACK_KDEVELOP.h"

#include <list>
#include <vector>
#include <forward_list>
 
#include "communication/agent_communicator_abstract.h"
#include "communication/world_communicator_abstract.h"
#include "../shared/yaml_parser.h"
#include "random.hpp"
#include "../plugins/abstract_agent_plugin.h"



class agent
{
public:
    agent (std::string name,  const std::unique_ptr< Parsed_Behavior >& behavior, const Parsed_World& world,bool noStart = false);
    agent ( const Parsed_World& world, bool noStart = false );
    ~agent();
    void start();
    void main_loop();
    void addPlugin( abstract_agent_plugin* plugin );
    /**
     * Setta il comunicatore
     */
    agent(const agent&)=delete;
    void set_communicator ( std::shared_ptr<agent_namespace::world_communicator_abstract>& communicator );
    
    /**
     * Setta l'identificatore
     */

    void setControlCommandIdentifier (std::string);
    
    bool initialized;
    	
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
	
	const world_sim_packet* agents;
	
	index_map map_statename_to_id;
	
	std::map<int,double> bonusVariables;
	std::map<std::string,int> map_bonus_variables_to_id;
	
	/**
	 * The value of control variables, update by controllers
	 */
	control_command_packet inputs;
	index_map map_inputs_name_to_id;
  
	objects_container objects;

	std::shared_ptr<agent_namespace::world_communicator_abstract> world_comm;
	
	/*!
	 * Moduli aggiuntivi utilizzabili stile plugin 
	 */
	std::vector <abstract_agent_plugin*> plugins;
	
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
};

#endif // AGENT_H
