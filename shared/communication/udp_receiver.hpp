#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H

#include "typedefs.h"

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

template <class T>
class udp_receiver
{
public:
  udp_receiver(boost::asio::io_service& io_service,
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

  T receive()
  {
	size_t size = socket_.receive_from(
		boost::asio::buffer(data_,max_length), sender_endpoint_
	);
	T inf; 
    boost::archive::text_iarchive archive(data_);
    archive >> inf;
      std::cout.write(data_, size);
      std::cout << std::endl;
	return inf;
  }

 
private:
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint sender_endpoint_;
  enum { max_length = 1024 };
  char data_[max_length];
};

#endif // TCP_RECEIVER_H
