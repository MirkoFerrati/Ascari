#ifndef TESTUDPSENDERRECEIVER_HPP
#define TESTUDPSENDERRECEIVER_HPP

#include <../shared/communication/udp_receiver.hpp>
#include <../shared/communication/udp_sender.hpp>



#include <vector>
#include <map>
#include <string>
#include "typedefs.h"

using namespace std;

class testUDPSenderReceiver
{
public:
	void test()
	{
	boost::asio::io_service service;//This must be written before any sender or receiver
	string multicast_address="239.255.0.1";
	int multicast_port=10000;
	udp_sender<string> sender(service, boost::asio::ip::address::from_string(multicast_address),multicast_port);
	udp_receiver<string> receiver(service,boost::asio::ip::address::from_string("0.0.0.0"), boost::asio::ip::address::from_string(multicast_address),multicast_port);
	string ciao= "ciao";
	sender.send(ciao);
	assert(receiver.receive().compare("ciao")==0);
	
	}
};

#endif // TESTUDPSENDERRECEIVER_HPP
