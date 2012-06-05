#include "simulator.h"
#include <udp_agent_communicator.h>
#include "typedefs.h"
#include "logog.hpp"

void simulator::create_communicator(int communicator_type)
{
    if (communicator_type==1)
    {
        communicator=new udp_agent_communicator();
    }
}


void simulator::main_loop()
{
    try {
        while (1) {
            communicator->send_broadcast(time++);
            //communicator->send_broadcast(states_index);
            //dynamic_module.evolve();
			sleep(1);
			//TODO: we don't need to copy, we need a function that overwrites only values inside the same memory!!
			commands=communicator->receive_control_commands();
        }
    }
    catch (const char* e)
    {
        ERR(e);
    }
}


simulator::simulator(int argc, char** argv)
{
	for (int i=1;i<argc;i++)
	{
		identifiers.push_back(argv[i]);
		std::cout<<identifiers[i-1]<<std::endl;
	}
}

simulator::~simulator()
{
	delete communicator;
}

void simulator::start_sim()
{
    time=0;
    //communicator->send_broadcast(time);
    main_loop();
}
