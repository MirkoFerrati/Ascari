#ifndef UDP_AGENT_ROUTER_H
#define UDP_AGENT_ROUTER_H
#include <../shared/communication/udp_receiver.hpp>
#include <../shared/communication/udp_sender.hpp>

#include <boost/thread.hpp>

#include <vector>
#include <map>
#include <string>
#include "typedefs.h"
#include <typedefs_topology.h>

typedef boost::shared_ptr<boost::thread> thread_ptr;

template <class T>
class Udp_agent_router
{
	boost::asio::io_service service;//This must be written before any sender or receiver
	
	udp_receiver<T> receiverTop;
	udp_sender<T> senderTop;
	thread_ptr   t;
	void service_thread(void) {
    int i=0;
	while (should_run || i++<20) {
		senderTop.send(receiverTop.receive());
    }
}
	bool should_run;

public:

Udp_agent_router(int sim_port,int agent_port):receiverTop(service,boost::asio::ip::address::from_string("0.0.0.0"), 
											  boost::asio::ip::address::from_string(MULTICAST_ADDRESS),sim_port),
											  senderTop(service, boost::asio::ip::address::from_string(MULTICAST_ADDRESS),agent_port)
{
	
}

void start_thread()
{
	should_run=true;
	t=thread_ptr(new boost::thread(boost::bind(&Udp_agent_router::service_thread,this)));
}

void join_thread()
{
	if (t)
		t->join();
}

void set_run(bool running)
{
	should_run=running;
}

~Udp_agent_router()
{
	should_run=false;
	t->interrupt();	
}


};

#endif // UDP_AGENT_ROUTER_H
