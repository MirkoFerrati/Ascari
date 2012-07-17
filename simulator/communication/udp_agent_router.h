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


class Udp_agent_router
{
	boost::asio::io_service service;//This must be written before any sender or receiver
	
	udp_receiver<topology_packet> receiverTop;
	udp_sender<topology_packet> senderTop;
	thread_ptr   t;
void service_thread(void);
	bool should_run;

public:
	Udp_agent_router();
	void set_run(bool running);
	void start_thread();
	~Udp_agent_router();
	void join_thread();
};

#endif // UDP_AGENT_ROUTER_H
