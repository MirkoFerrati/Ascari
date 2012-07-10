#ifndef UDP_ASYNC_RECEIVER_HPP
#define UDP_ASYNC_RECEIVER_HPP

#include "typedefs.h"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include <typedefs_topology.h>

class udp_async_receiver
{
public:
  receiver(boost::asio::io_service& io_service,
      const boost::asio::ip::address& listen_address,
      const boost::asio::ip::address& multicast_address,int multicast_port)
    : socket_(io_service)
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
  
  void receive()
  {
	socket_.async_receive_from(
        boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), sender_endpoint_,
        boost::bind(&udp_async_receiver::handle_receive_from, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive_from(const boost::system::error_code& error,
      size_t bytes_recvd)
  {
    if (!error)
    {
      
	  
	  try
        {
            std::string archive_data(&inbound_data_[header_length], inbound_data_.size()-header_length);
            std::istringstream archive_stream(archive_data);
            boost::archive::text_iarchive archive(archive_stream);
            archive >> tp;
        }
        catch (std::exception& e)
        {
            // Unable to decode data.
            boost::system::error_code error(boost::asio::error::invalid_argument);
			std::cout.write(inbound_data_, bytes_recvd);
			std::cout << std::endl;
// 			ERR("pacchetto sbagliato: %s",inbound_data_);
            throw "Problema nella ricezione di un pacchetto";
        }
        
        //TODO: merge tp into our map

      socket_.async_receive_from(
          boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), sender_endpoint_,
          boost::bind(&udp_async_receiver::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  }

private:
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint sender_endpoint_;
  char inbound_data_[MAX_PACKET_LENGTH];
      /// The size of a fixed length header.
    enum { header_length = 8 };
	topology_packet tp;
};


#endif // UDP_ASYNC_RECEIVER_HPP
