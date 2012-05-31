#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "communication/agent_communicator_abstract.h"



class simulator
{
public:
	simulator();
	void start_sim();
	void create_communicator(int communicator_type);
private:
	
	agent_communicator_abstract* communicator;
	
	 
	void main_loop();
};

#endif // SIMULATOR_H
