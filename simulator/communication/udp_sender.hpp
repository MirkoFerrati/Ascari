#ifndef TCP_SENDER_H
#define TCP_SENDER_H

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
class udp_sender
{
public:
  udp_sender(boost::asio::io_service& io_service,
      const boost::asio::ip::address& multicast_address,int multicast_port)
    : endpoint_(multicast_address, multicast_port),
      socket_(io_service, endpoint_.protocol())
  {
  }
  void send(T inf)
  {
	// Serialize the data first so we know how large it is.
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    
	archive << inf;
    
	outbound_data_ = archive_stream.str();

    // Format the header.
    std::ostringstream header_stream;
    header_stream << std::setw(header_length)
      << std::hex << outbound_data_.size();
    if (!header_stream || header_stream.str().size() != header_length)
    {
      throw
		"error during packet creation";
    }
    outbound_header_ = header_stream.str();

    // Write the serialized data to the socket. We use "gather-write" to send
    // both the header and the data in a single write operation.
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(outbound_header_));
    buffers.push_back(boost::asio::buffer(outbound_data_));
   
    socket_.async_send_to(
        //boost::asio::buffer(message_), endpoint_,
		buffers, endpoint_,
        boost::bind(&udp_sender::handle_send_to, this,
          boost::asio::placeholders::error));
	
  }




private:
	
	  void handle_send_to(const boost::system::error_code& error)
  {
	if (error)
		throw "errore nell'invio";
  }

	
  /// The size of a fixed length header.
  enum { header_length = 8 };

  /// Holds an outbound header.
  std::string outbound_header_;

  /// Holds the outbound data.
  std::string outbound_data_;
 
  boost::asio::ip::udp::endpoint endpoint_;
  boost::asio::ip::udp::socket socket_;

   

};


#endif // TCP_SENDER_H
