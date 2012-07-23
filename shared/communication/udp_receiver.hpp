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

#include <logog.hpp>

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
		inbound_data_.resize(MAX_PACKET_LENGTH);
    }

    T receive(bool print=false)
    {
        using namespace std;
        //unsigned int size=socket_.available();//TODO: what if we have more than one packet in the socket buffer?
        // Start to receive the data.
        //inbound_data_.resize(1000);

        unsigned int size=socket_.receive(boost::asio::buffer(inbound_data_));
		
		if (print)
		{	
		for (unsigned int i=0;i<size;i++)
			cout<<inbound_data_[i];
		cout<<endl;
		}
        try
        {
            std::string archive_data(&inbound_data_[header_length], inbound_data_.size()-header_length);
            std::istringstream archive_stream(archive_data);
            boost::archive::text_iarchive archive(archive_stream);
            T t;
            archive >> t;
            return t;
        }
        catch (std::exception& e)
        {
            // Unable to decode data.
            boost::system::error_code error(boost::asio::error::invalid_argument);
			for (unsigned int i=0;i<size;i++)
				cout<<inbound_data_[i];
			cout<<endl;
// 			ERR("pacchetto sbagliato: %s",inbound_data_);
            throw "Problema nella ricezione di un pacchetto";
        }

    }

    ~udp_receiver() {
        socket_.close();
		inbound_data_.clear();
    }

private:




    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint sender_endpoint_;
    enum { max_length = MAX_PACKET_LENGTH };
    /// Holds an inbound header.


    /// The size of a fixed length header.
    enum { header_length = 8 };

    char inbound_header_[header_length];

    /// Holds the inbound data.
    std::vector<char> inbound_data_;
    char data_[max_length];
};

#endif // TCP_RECEIVER_H
