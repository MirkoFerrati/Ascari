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
        using namespace std;
        //unsigned int size=socket_.available();//TODO: what if we have more than one packet in the socket buffer?
        // Start to receive the data.
        inbound_data_.resize(1000);

        int size=socket_.receive(boost::asio::buffer(inbound_data_));
        try
        {
            std::string archive_data(&inbound_data_[header_length], inbound_data_.size());
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
            throw "Problema nella ricezione di un pacchetto";
        }

    }

    ~udp_receiver() {
        socket_.close();
    }

private:




    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint sender_endpoint_;
    enum { max_length = 1024 };
    /// Holds an inbound header.


    /// The size of a fixed length header.
    enum { header_length = 8 };

    char inbound_header_[header_length];

    /// Holds the inbound data.
    std::vector<char> inbound_data_;
    char data_[max_length];
};

#endif // TCP_RECEIVER_H
