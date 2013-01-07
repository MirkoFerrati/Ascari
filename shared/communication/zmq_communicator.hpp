
#include <zmq.hpp>
#include <vector>
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
#include <boost/archive/binary_iarchive.hpp>
#include <iomanip>
#include <boost/serialization/vector.hpp>
#include <boost/iostreams/stream.hpp>
#include <logog.hpp>
#include "global.h"

enum Synctypes {
    NO_SYNC,
    WAIT_SYNC,
    ASK_SYNC
};


/**
 * 
 * Receiver and sender are (optionally) synchronized
 * \param infos sent is not modified, data received are returned in a std::vector 
 * Everything is copied in a buffer, sometimes more than once
 * TODO(Mirko): implement zero-copy
 */
template <class receive_type, class send_type, int sock_send_type, int sock_recv_type, Synctypes sync = NO_SYNC>
class zmq_communicator
{



public:
    zmq_communicator() : sender_socket (static_zmq::context, sock_send_type), receiver_socket (static_zmq::context, sock_recv_type) {
        assert (sock_recv_type == ZMQ_PULL || sock_recv_type == ZMQ_SUB);
        assert (sock_send_type == ZMQ_PUSH || sock_send_type == ZMQ_PUB);
        receive_buffer.rebuild (MAX_PACKET_LENGTH);
        send_buffer.rebuild (MAX_PACKET_LENGTH);

        if (sync == WAIT_SYNC) {
            sync_socket = new zmq::socket_t (static_zmq::context, ZMQ_REP);
        } else if (sync == ASK_SYNC) {
            sync_socket = new zmq::socket_t (static_zmq::context, ZMQ_REQ);
        }
    }

protected:
    /**
     * This function will block till the syncing phase is over, if the class was created with a syncing policy!
     * */
	bool init_full(std::string owner_name,std::string receiver_protocol, std::string sender_protocol,
				   std::string sync_protocol="",unsigned int expected_senders=1,bool bind_receiver=false, bool bind_sender=true) {
        this->expected_senders=expected_senders;
		if (bind_receiver)
		{
			receiver_socket.bind(receiver_protocol.c_str());
		}
		else
		{
			receiver_socket.connect(receiver_protocol.c_str());
		}
		if (bind_sender)
		{
			sender_socket.bind(sender_protocol.c_str());
		}
		else
		{
			sender_socket.connect(sender_protocol.c_str());
		}
		if (sock_recv_type==ZMQ_SUB)
		{
		  receiver_socket.setsockopt(ZMQ_SUBSCRIBE,"",0);
		}
		results.resize (expected_senders);
        this->owner_name = owner_name;
        if (sync == WAIT_SYNC) {
          sync_socket->bind (!sync_protocol.compare("")?SYNC_PROTOCOL:sync_protocol.c_str());

            std::cout << owner_name << " waiting for clients..." << std::endl;
            unsigned int subscribers = 0;
            while (subscribers < expected_senders) {
                //  - wait for synchronization request
                zmq::message_t message_tmp;
                sync_socket->recv (&message_tmp);
                std::string name (static_cast<char*> (message_tmp.data()), message_tmp.size());

                std::string result = "one more client connected to server: "; //non so chi si sia connesso, sono tutti uguali
                //  - send synchronization reply
                result.append (name);
                zmq::message_t message (result.size()+1);
                memcpy (message.data(), result.data(), result.size()+1);

                bool rc = sync_socket->send (message); assert(rc);
                std::cout << result << std::endl;
                subscribers++;
            }
        } else if (sync == ASK_SYNC) {
            /** TODO(Mirko): implementare il modello qui sotto, e' più robusto
             * A more robust model could be:

                Publisher opens PUB socket and starts sending "Hello" messages (not data).
                Subscribers connect SUB socket and when they receive a Hello message they tell the publisher via a REQ/REP socket pair.
                When the publisher has had all the necessary confirmations, it starts to send real data.
              */
            sync_socket->connect (!sync_protocol.compare("")?SYNC_PROTOCOL:sync_protocol.c_str());

            std::cout << owner_name << " connecting to server..." << std::endl;

            zmq::message_t message (owner_name.size());
            memcpy (message.data(), owner_name.data(), owner_name.size());
            bool rc = sync_socket->send (message); assert(rc);
            message.rebuild (MAX_PACKET_LENGTH);
            sync_socket->recv (&message);

           // std::cout << static_cast<char*> (message.data()) << std::endl;

        }
        return true;
    }
    
    
    std::vector<receive_type> receive()
    {
        unsigned int subscribers = 0;
	receive_type packet;
	results.clear();
	while (subscribers < expected_senders) {
		
        receiver_socket.recv(&receive_buffer);
	//boost::iostreams::stream_buffer<boost::iostreams::basic_array_source<char> > buffer( (char*)receive_buffer.data(), receive_buffer.size());
	//boost::archive::binary_iarchive archive(buffer, boost::archive::no_header);
        char* receive=reinterpret_cast<char*>(receive_buffer.data());
	//std::cout<<receive<<std::endl;
	std::istringstream receive_stream(
	std::string(receive,receive_buffer.size()));
	boost::archive::text_iarchive archive(receive_stream);
	archive >> packet;
        results.push_back(packet);
	subscribers++;
    }
    return results;
    };
    void send(send_type const & infos)
    {
        std::ostringstream archive_stream;
	boost::archive::text_oarchive archive(archive_stream);
	archive << infos;
	send_buffer.rebuild(archive_stream.str().length()+1);
	reinterpret_cast<char*>( memcpy(send_buffer.data(), archive_stream.str().data(),archive_stream.str().length()+1));
	//std::cout<<temp<<std::endl;
	sender_socket.send(send_buffer);
    };

	
private:
    zmq::socket_t sender_socket, receiver_socket, *sync_socket;
    zmq::message_t receive_buffer, send_buffer;
    std::vector <receive_type> results;
    std::string owner_name;
	unsigned int expected_senders;
};
