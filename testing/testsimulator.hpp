#ifndef TESTSIMULATOR_HPP
#define TESTSIMULATOR_HPP
#include "testClass.h"
#include <map>
#include <communication/udp_receiver.hpp>
#include <communication/udp_sender.hpp>


using namespace std;

class testSimulator:public testClass
{
public:
	void test()
	{
	  agents_name_to_states states;
	  
	  boost::asio::io_service service;
	  
	  
	  udp_receiver<agents_name_to_states> state_receiver(service,boost::asio::ip::address::from_string("0.0.0.0"),boost::asio::ip::address::from_string(MULTICAST_ADDRESS),MULTICAST_PORT);
	  udp_sender<control_command_packet> command_sender(service,boost::asio::ip::address::from_string(MULTICAST_ADDRESS),SIMULATOR_PORT);
	  
	  int i=0;
	  while(i<10){
	    i++;
	    
	    states=state_receiver.receive();
	  
	    map<string,agent_state_packet>::iterator it=states.internal_map.begin();
	    
	    string id= (*it).first;
	    
	    control_command_packet command;
	    
	    command.identifier=id;
	    command.command[0]=3;
	    
	    command_sender.send(command);
	    
	    cout << states.internal_map[id].state[0]<<endl;
	    
	    
	  }
	}
};

#endif // TESTSIMULATOR_HPP
