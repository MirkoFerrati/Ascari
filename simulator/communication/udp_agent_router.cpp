#include "udp_agent_router.h"


Udp_agent_router::Udp_agent_router():receiverTop(service,boost::asio::ip::address::from_string("0.0.0.0"), 
											  boost::asio::ip::address::from_string(MULTICAST_ADDRESS),SIMULATOR_ROUTE_PORT),
											  senderTop(service, boost::asio::ip::address::from_string(MULTICAST_ADDRESS),AGENT_ROUTE_PORT)
{
	
}

void Udp_agent_router::start_thread()
{
	should_run=true;
	t=thread_ptr(new boost::thread(boost::bind(&Udp_agent_router::service_thread,this)));
}

void Udp_agent_router::join_thread()
{
	if (t)
		t->join();
}

void Udp_agent_router::set_run(bool running)
{
	should_run=running;
}

Udp_agent_router::~Udp_agent_router()
{
	should_run=false;
	t->interrupt();	
}

void Udp_agent_router::service_thread(void) {
    int i=0;
	while (should_run || i++<20) {
		senderTop.send(receiverTop.receive());
    }
}

