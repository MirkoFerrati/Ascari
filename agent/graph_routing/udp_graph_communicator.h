#ifndef UDP_GRAPH_COMMUNICATOR_H
#define UDP_GRAPH_COMMUNICATOR_H
#include "agent_communicator_abstract.h"
#include <boost/signals2/mutex.hpp>
#include "typedefs.h"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include <typedefs_topology.h>
#include <communication/udp_sender.hpp>
#include <boost/thread.hpp>

typedef boost::shared_ptr<boost::thread> thread_ptr;


class Udp_graph_communicator : public agent_communicator_abstract
{
public:
	/*!
	 * Questa classe non garantisce che le reference a \param tp rimangano valide non appena udp_graph_communicator::startReceive
	 * viene avviata
	 */ 
    Udp_graph_communicator(boost::signals2::mutex& mutex, graph_packet* gi, boost::asio::io_service& io_service);
    void send();
    void startReceive();
    ~Udp_graph_communicator();

private:
	void service_thread(void);
    void handle_receive_from(const boost::system::error_code& error,size_t bytes_recvd);
    boost::signals2::mutex& mutex;
	graph_packet* tp;
	boost::asio::ip::udp::socket socket_;
	udp_sender<graph_packet> sender;
    boost::asio::ip::udp::endpoint listen_endpoint_;
    char inbound_data_[MAX_PACKET_LENGTH];
    enum { header_length = 8 };
	thread_ptr   t;
	bool should_run;
	boost::asio::io_service& _io_service;
    graph_packet input_map_tp;
    graph_packet output_map_tp;
	
    bool mutex_is_mine;
};

#endif // UDP_GRAPH_COMMUNICATOR_H