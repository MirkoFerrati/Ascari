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

class agent
{
public:
	agent(std::string name);
	~agent();
	
protected:
	/**
	 * The continuos state of the agent, used in the dynamic
	 */
	agent_state state;
	
	automaton_state discreteState;
	
	//in dummy agent, we will initialize a nautomaton, in agent an automaton
	automatonAbstract* automaton;
	
	
	/**
	 * every possible controller must be created and pushed inside this vector from the constructor
	 * and never modified again
	 */
	std::vector<controller> controllers;
	
	//in dummy we will not use this, since it is used for inter-agent communication
	//Note that we need two versions, one for net and one for shared memory
	agent_communicator_abstract* agent_comm;
	
	//in dummy this will be a way to access identifier informations, in agent this will communicate with simulator (tcp or shared memory)
	world_communicator_abstract* world_comm;
	
	//in dummy agent this will not be initialized
	identifier_module* idModule;
	
	std::string identifier;
	
	void main_loop();
	
};

#endif // AGENT_H
