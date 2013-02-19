#ifndef UDP_AGENT_COMMUNICATOR_H
#define UDP_AGENT_COMMUNICATOR_H
#include "agent_communicator_abstract.h"
#include <boost/signals2/mutex.hpp>
#include "typedefs.h"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include <typedefs_topology.h>
#include <communication/udp_sender.hpp>


class udp_agent_communicator : public agent_namespace::agent_communicator_abstract
{
public:
	/*!
	 * Questa classe non garantisce che le reference a \param tp rimangano valide non appena udp_agent_communicator::startReceive
	 * viene avviata
	 */ 
    udp_agent_communicator(boost::signals2::mutex& mutex, topology_packet* tp, boost::asio::io_service& io_service);
    void send(bool printDebug=false);
    void startReceive(bool printDebug = false);

private:
    void handle_receive_from(const boost::system::error_code& error,size_t bytes_recvd);
    boost::signals2::mutex& mutex;
	    topology_packet* tp;
	boost::asio::ip::udp::socket socket_;
	udp_sender<topology_packet> sender;
    boost::asio::ip::udp::endpoint listen_endpoint_;
    char inbound_data_[MAX_PACKET_LENGTH];
    enum { header_length = 8 };

    topology_packet input_map_tp;
    topology_packet output_map_tp;
	bool printDebug;
    bool mutex_is_mine;
};

#endif // UDP_AGENT_COMMUNICATOR_H
