
#include <zmq.hpp>
#include <vector>
#include "typedefs.h"

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
template <class receive_type, class send_type, int sock_send_type, int sock_recv_type, Synctypes sync = NO_SYNC, unsigned int expected_senders=1>
class zmq_communicator
{



public:
    zmq_communicator() : context (1), sender_socket (context, sock_send_type), receiver_socket (context, sock_recv_type) {
        assert (sock_recv_type == ZMQ_PULL || sock_recv_type == ZMQ_SUB);
        assert (sock_send_type == ZMQ_PUSH || sock_send_type == ZMQ_PUB);
        receive_buffer.rebuild (MAX_PACKET_LENGTH);
        send_buffer.rebuild (MAX_PACKET_LENGTH);

        if (sync == WAIT_SYNC) {
            sync_socket = new zmq::socket_t (context, ZMQ_REP);
        } else if (sync == ASK_SYNC) {
            sync_socket = new zmq::socket_t (context, ZMQ_REQ);
        }
    }
    
    /**
     * This function will block till the syncing phase is over, if the class was created with a syncing policy!
     * */
    bool init (std::string owner_name,std::string receiver_protocol,std::string sender_protocol,std::string sync_protocol="") {
        receiver_socket.connect(receiver_protocol);
	sender_socket.bind(sender_protocol);
        results.resize (expected_senders);
        this->owner_name = owner_name;
        if (sync == WAIT_SYNC) {
          sync_socket->bind (sync_protocol.compare("")?SYNC_PROTOCOL:sync_protocol);

            std::cout << owner_name << " waiting for clients..." << std::endl;
            int subscribers = 0;
            while (subscribers < expected_senders) {

                //  - wait for synchronization request
                zmq::message_t message_tmp;
                sync_socket->recv (&message_tmp);
                std::string name (static_cast<char*> (message_tmp.data()), message_tmp.size());

                std::string string = "one more client connected to server: "; //non so chi si sia connesso, sono tutti uguali
                //  - send synchronization reply
                string.append (name);
                zmq::message_t message (string.size());
                memcpy (message.data(), string.data(), string.size());

                bool rc = sync_socket->send (message);
                std::cout << message.data() << std::endl;
                subscribers++;
            }
        } else if (sync == ASK_SYNC) {
            /** TODO(Mirko): implementare il modello qui sotto, è più robusto
             * A more robust model could be:

                Publisher opens PUB socket and starts sending "Hello" messages (not data).
                Subscribers connect SUB socket and when they receive a Hello message they tell the publisher via a REQ/REP socket pair.
                When the publisher has had all the necessary confirmations, it starts to send real data.
              */
            sync_socket->connect (sync_protocol.compare("")?SYNC_PROTOCOL:sync_protocol);

            std::cout << owner_name << " connecting to server..." << std::endl;

            zmq::message_t message (owner_name.size());
            memcpy (message.data(), owner_name.data(), owner_name.size());
            bool rc = sync_socket->send (message);
            message.rebuild (MAX_PACKET_LENGTH);
            sync_socket->recv (&message);

            std::cout << message.data() << std::endl;

        }
    }
    std::vector<receive_type> receive()
    {
        int subscribers = 0;
	receive_type packet;
	results.clear();
	while (subscribers < expected_senders) {
		
        receiver_socket.recv(&receive_buffer);
		boost::iostreams::stream_buffer<boost::iostreams::basic_array_source<char> > buffer( (char*)receive_buffer.data(), receive_buffer.size());
		boost::archive::binary_iarchive archive(buffer, boost::archive::no_header);
        archive >> packet;
        results.push_back(packet);
    }
    return results;
    };
    void send(send_type const & infos)
    {
        std::ostringstream archive_stream;
	boost::archive::text_oarchive archive(archive_stream);
	archive << infos;
	send_buffer.rebuild(archive_stream.str().length());
	memcpy(send_buffer.data(), archive_stream.str().data(),archive_stream.str().length());
	sender_socket.send(send_buffer);
    };

private:
    zmq::context_t context;
    zmq::socket_t sender_socket, receiver_socket, *sync_socket;
    zmq::message_t receive_buffer, send_buffer;
    std::vector <receive_type> results;
    std::string owner_name;
};
