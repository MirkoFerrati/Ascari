#ifndef TESTAGENTUDPCOMMUNICATOR_HPP
#define TESTAGENTUDPCOMMUNICATOR_HPP
#include "testClass.h"
#include <../shared/communication/udp_receiver.hpp>
#include <../shared/communication/udp_sender.hpp>



#include <vector>
#include <map>
#include <string>
#include "typedefs.h"
#include <typedefs_topology.h>
#include <../simulator/communication/udp_agent_router.hpp>


class testAgentUDPCommunicator: public testClass
{
    void test()
	{
	boost::asio::io_service service;//This must be written before any sender or receiver
	string multicast_address=MULTICAST_ADDRESS;
	int multicast_port=MULTICAST_PORT;
	udp_sender<int> sender(service, boost::asio::ip::address::from_string(multicast_address),multicast_port);
// 	udp_receiver<topology_packet> receiverTop(service,boost::asio::ip::address::from_string("0.0.0.0"), 
// 											  boost::asio::ip::address::from_string(multicast_address),SIMULATOR_ROUTE_PORT);
// 	udp_sender<topology_packet> senderTop(service, boost::asio::ip::address::from_string(multicast_address),AGENT_ROUTE_PORT);
	Udp_agent_router<topology_packet> r(SIMULATOR_ROUTE_PORT,AGENT_ROUTE_PORT);
		
	sender.send(5);
// 	int i=0;
// 	while (i++<12)
// 		senderTop.send(receiverTop.receive());
// 	
		r.start_thread();
		r.join_thread();
	}
};

#endif // TESTAGENTUDPCOMMUNICATOR_HPP