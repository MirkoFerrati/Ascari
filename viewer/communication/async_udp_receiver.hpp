#ifndef ASYNC_UDP_RECEIVER_HPP
#define ASYNC_UDP_RECEIVER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <boost/lexical_cast.hpp>
#include <vector>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iomanip>
#include <boost/serialization/vector.hpp>

#include <define.h>

class async_udp_receiver
{
public:
    async_udp_receiver(boost::asio::io_service& io_service,
                       const boost::asio::ip::address& listen_address,
                       const boost::asio::ip::address& multicast_address,int multicast_port,
					   std::vector<char>& data_ref
  					)
            : socket_(io_service),inbound_data_(data_ref)
    {
        // Create the socket so that multiple may be bound to the same address.
        boost::asio::ip::udp::endpoint listen_endpoint(
            listen_address, multicast_port);
        socket_.open(listen_endpoint.protocol());
        socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        socket_.bind(listen_endpoint);

        // Join the multicast group.
        socket_.set_option(
            boost::asio::ip::multicast::join_group(multicast_address));
    }

    void handle_receive_from(const boost::system::error_code& error,
                             size_t /*bytes_recvd*/)
    {
        if (!error)
        {
// 			for (int i =0;i<bytes_recvd;i++)
// 				std::cout<<inbound_data_[i];
//             std::cout<<std::endl;
            socket_.async_receive_from(
                boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), sender_endpoint_,
                boost::bind(&async_udp_receiver::handle_receive_from, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        }
    }

    void receive()
    {
        // socket_.receive_from(
        //     boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), sender_endpoint_);
		socket_.async_receive_from(
                boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), sender_endpoint_,
                boost::bind(&async_udp_receiver::handle_receive_from, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
       //socket_.get_io_service().run();
    }

    ~async_udp_receiver() {
        socket_.close();
    }

private:




    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint sender_endpoint_;

    /// Holds the inbound data.
    std::vector<char>& inbound_data_;
};

#endif // ASYNC_UDP_RECEIVER_HPP
