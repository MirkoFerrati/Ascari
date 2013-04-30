#ifndef TESTUDPSENDERRECEIVER_HPP
#define TESTUDPSENDERRECEIVER_HPP

#include <../shared/communication/udp_receiver.hpp>
#include <../shared/communication/udp_sender.hpp>
#include <types/agents_name_to_states.h>



#include <vector>
#include <map>
#include <string>

#include "testClass.h"

using namespace std;

class testUDPSenderReceiver:public testClass
{
public:
	void test()
	{
	boost::asio::io_service service;//This must be written before any sender or receiver
	string multicast_address="239.255.0.1";
	int multicast_port=10000;
	udp_sender<agents_name_to_states> sender(service, boost::asio::ip::address::from_string(multicast_address),multicast_port);
	udp_receiver<agents_name_to_states> receiver(service,boost::asio::ip::address::from_string(SOCKET_BINDING), boost::asio::ip::address::from_string(multicast_address),multicast_port);
	
		agents_name_to_states states_index;
	    agent_state_packet agent_packet;
        agent_packet.identifier="pippo";
        agent_packet.state.insert(make_pair(0,0));
		agent_packet.state.insert(make_pair(1,1));
		
        states_index.internal_map.insert(make_pair("pippo",agent_packet));

		sender.send(states_index);
		assert(receiver.receive().internal_map.at("pippo").state.at(1)==1);
		
	//string ciao= "ciao";
	//sender.send(ciao);
	//assert(receiver.receive().compare("ciao")==0);
	
	}
};

#endif // TESTUDPSENDERRECEIVER_HPP
