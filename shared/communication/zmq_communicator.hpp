#ifndef ZMQ_COMMUNICATOR_HPP
#define ZMQ_COMMUNICATOR_HPP
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
#include <mutex>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <iomanip>
#include <boost/serialization/vector.hpp>
#include <boost/iostreams/stream.hpp>
#include <logog.hpp>
#include "global.h"

enum Synctypes
{
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
template <typename receive_type, typename send_type, int sock_send_type, int sock_recv_type, Synctypes sync = NO_SYNC>
class zmq_communicator
{

public:
    zmq_communicator() : sender_socket ( static_zmq::context, sock_send_type ), receiver_socket ( static_zmq::context, sock_recv_type )
    {
        assert ( sock_recv_type == ZMQ_PULL || sock_recv_type == ZMQ_SUB );
        assert ( sock_send_type == ZMQ_PUSH || sock_send_type == ZMQ_PUB );
        initialized = false;
        receive_buffer.rebuild ( MAX_PACKET_LENGTH );
        send_buffer.rebuild ( MAX_PACKET_LENGTH );
        sync_socket = 0;
        int temp = 0;
        receiver_socket.setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );
        sender_socket.setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );
        if ( sync == WAIT_SYNC )
        {
            sync_socket = new zmq::socket_t ( static_zmq::context, ZMQ_REP );
            sync_socket->setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );
        }
        else if ( sync == ASK_SYNC )
        {
            sync_socket = new zmq::socket_t ( static_zmq::context, ZMQ_REQ );
            sync_socket->setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );
        }
    }

    ~zmq_communicator()
    {
        receiver_socket.close();
        sender_socket.close();
        if ( sync_socket )
        {
            sync_socket->close();
            delete sync_socket;
        }
    }

protected:

    std::list<std::string> clients;

    void setClientsName ( const std::list<std::string>& clients )
    {
        clientsNamed = true;
        this->clients = clients;
    }


    bool waitSync()
    {

        std::cout << owner_name << " waiting for clients..." << std::endl;
        unsigned int subscribers = 0;
        while ( subscribers < expected_senders )
        {
            //  - wait for synchronization request
            zmq::message_t message_tmp;
            try
            {
                sync_socket->recv ( &message_tmp );
                std::cout << "ricevuto un nuovo client" << std::endl;
            }
            catch ( zmq::error_t ex )
            {
                if ( zmq_errno() == EINTR )
                {
                    WARN ( "programma terminato", NULL );
                    break;
                }
                else
                    throw ex;
            }
            std::string name ( static_cast<char*> ( message_tmp.data() ), message_tmp.size() );

            std::string result = "one more client connected to ";
            std::cout << result << " server: " << name << std::endl;
            bool rejected = true;
            if ( clientsNamed )
            {
                auto client = std::find ( clients.begin(), clients.end(), name );
                if ( client != clients.end() )
                {
                    clients.erase ( client );
                    rejected = false;
                }
                else
                {
		  rejected=true;
                }
            }
            agent_simulator_handshake_packet infos;
            if ( !rejected || !clientsNamed )
            {
                //  - send synchronization reply
                result.append ( owner_name );
                result.append ( " accepted " );
                result.append ( name );
                infos.accepted = true;
                infos.message = result;
                std::cout << name << " accepted" << std::endl;
		subscribers++;
            }
            else
            {
                result.append ( owner_name );
                result.append ( " rejected " );
                result.append ( name );
                infos.accepted = false;
                infos.message = result;
                std::cout << name << " rejected" << std::endl;

            }

            std::ostringstream archive_stream;
            boost::archive::text_oarchive archive ( archive_stream );
            archive << infos;
            zmq::message_t message ( archive_stream.str().length() + 1 );
            reinterpret_cast<char*> ( memcpy ( message.data(), archive_stream.str().data(), archive_stream.str().length() + 1 ) );
            //std::cout<<temp<<std::endl;
            try
            {
                bool rc = sync_socket->send ( message );
                assert ( rc );
                
            }
            catch ( zmq::error_t ex )
            {
                if ( zmq_errno() == EINTR )
                {
                    WARN ( "programma terminato", NULL );
                    break;
                }
            }
        }
        return true;
    }


    bool askSync()
    {
        /** TODO(Mirko): implementare il modello qui sotto, e' più robusto
              * A more robust model could be:

                 Publisher opens PUB socket and starts sending "Hello" messages (not data).
                 Subscribers connect SUB socket and when they receive a Hello message they tell the publisher via a REQ/REP socket pair.
                 When the publisher has had all the necessary confirmations, it starts to send real data.
               */
        std::cout << owner_name << " connecting to server..." << std::endl;
	 agent_simulator_handshake_packet infos;
       bool exit=false;
	while(!exit){
        zmq::message_t message ( owner_name.size() );
        memcpy ( message.data(), owner_name.data(), owner_name.size() );
        bool rc = sync_socket->send ( message );
        assert ( rc );
        message.rebuild ( MAX_PACKET_LENGTH );
        try
        {
            bool rc = sync_socket->recv ( &message,ZMQ_NOBLOCK );
	    if (rc)
	    {
            char* receive = reinterpret_cast<char*> ( message.data() );
            //std::cout<<receive<<std::endl;
            std::istringstream receive_stream (
                std::string ( receive, receive_buffer.size() ) );
            boost::archive::text_iarchive archive ( receive_stream );
            archive >> infos;
	    exit=true;
	    }
	    else 
	    {
	      usleep(250000);
	      continue;
	    }
	   
        }
        catch ( zmq::error_t ex )
        {
            if ( zmq_errno() == EINTR )
                WARN ( "programma terminato", NULL );
            return false;
        }
	}
        std::cout << infos.message << std::endl;
        if ( !infos.accepted )
        {
            return false;
        }
        else
            return true;
    }

    /**
     * This function will block till the syncing phase is over, if the class was created with a syncing policy!
     * */
    bool init_full ( std::string owner_name,bool filter, std::string receiver_protocol, std::string sender_protocol,
                     std::string sync_protocol = "", unsigned int expected_senders = 1, bool bind_receiver = false, bool bind_sender = true )
    {
        this->expected_senders = expected_senders;
        if ( clientsNamed )
        {
            if ( expected_senders != clients.size() )
            {
                ERR ( "clients name not coerent with expected_senders size", NULL );
                throw "clients name not coerent with expected_senders size";
            }
        }
        if ( bind_receiver )
        {
            receiver_socket.bind ( receiver_protocol.c_str() );
        }
        else
        {
            receiver_socket.connect ( receiver_protocol.c_str() );
        }
        if ( bind_sender )
        {
            sender_socket.bind ( sender_protocol.c_str() );
        }
        else
        {
            sender_socket.connect ( sender_protocol.c_str() );
        }
        if ( sock_recv_type == ZMQ_SUB )
        {
	  if (filter)
            receiver_socket.setsockopt ( ZMQ_SUBSCRIBE, owner_name.c_str(), owner_name.length()+1 );
	  else
	    receiver_socket.setsockopt ( ZMQ_SUBSCRIBE, "", 0 );
        }

        results.resize ( expected_senders );
        this->owner_name = owner_name;
        if ( sync == WAIT_SYNC )
        {
            sync_socket->bind ( !sync_protocol.compare ( "" ) ? SYNC_PROTOCOL : sync_protocol.c_str() );
            waitSync();
	    sync_socket->close();
	    delete sync_socket;

        }
        else if ( sync == ASK_SYNC )
        {

            sleep ( 1 );
            sync_socket->connect ( !sync_protocol.compare ( "" ) ? SYNC_PROTOCOL : sync_protocol.c_str() );

            if ( !askSync() )
            {
                ERR ( "sync refused, check configuration and previous messages", NULL );
                abort();
            };
	        sync_socket->close();
	    delete sync_socket;

        }
        initialized = true;
        return true;
    }

public:

#ifdef ZMQDEBUG
    std::mutex check_for_unique_call;

#endif //ZMQDEBUG

    std::vector<receive_type> receive ( int flags = 0 )
    {
        if ( !initialized )
        {
            ERR ( "receive chiamata senza avere inizializzato il communicator", NULL );
            throw "receive chiamata senza avere inizializzato il communicator";
        }
#ifdef ZMQDEBUG
        bool fail = !check_for_unique_call.try_lock();
        if ( fail )
        {
            ERR ( "receive e' gia' in esecuzione,probabilmente ci sono due thread concorrenti che chiamano la stessa receive", NULL );
        }
#endif //ZMQDEBUG
        unsigned int subscribers = 0;
        receive_type packet;
        results.clear();
        while ( subscribers < expected_senders )
        {
bool rc=receiver_socket.recv(&receive_buffer,flags);
	if (!rc)
	{
	    ERR("brutte cose",NULL);
	}
            char* receive = reinterpret_cast<char*> ( receive_buffer.data() );
	    //TODO(Mirko): codice brutto, migliorare
//             std::cout<<receive<<std::endl;
	    std::string tmp;
	    std::istringstream iss(receive);
	    iss >> tmp; 
// 	    std::cout<<tmp<<std::endl;
//             std::istringstream receive_stream (tmp);
               // std::string ( receive, receive_buffer.size() ) );
            boost::archive::text_iarchive archive ( iss );
            archive >> packet;
            results.push_back ( packet );
            subscribers++;
        }
#ifdef ZMQDEBUG
        check_for_unique_call.unlock();

#endif //ZMQDEBUG
        return results;

    };
    void send ( send_type const& infos, const target_abstract & target="all" )
    {
        if ( !initialized )
        {
            ERR ( "send chiamata senza avere inizializzato il communicator", NULL );
            throw "send chiamata senza avere inizializzato il communicator";
        }
        std::ostringstream archive_stream;
        boost::archive::text_oarchive archive ( archive_stream );
        archive << infos;
	std::string tmp=target;
	tmp.append(" ");
	tmp.append(archive_stream.str());
	//send_buffer.rebuild ( archive_stream.str().length() + 1+target.length()+2 );
	send_buffer.rebuild(tmp.length()+1);
	memcpy(send_buffer.data(), tmp.c_str(),tmp.length()+1);
	//((char*) send_buffer.data())[target.length()]=' ';
         //memcpy (send_buffer.data(), archive_stream.str().data(), archive_stream.str().length() + 1 ) ;
	 std::string temp=reinterpret_cast<char*> (send_buffer.data());
//         std::cout<<temp<<std::endl;
        
	sender_socket.send ( send_buffer );
    };


private:
    zmq::socket_t sender_socket, receiver_socket, *sync_socket;
    zmq::message_t receive_buffer, send_buffer;
    std::vector <receive_type> results;
    std::string owner_name;
    unsigned int expected_senders;
    bool initialized;
    bool clientsNamed;
};


template <typename receive_type,int sock_recv_type>
class zmq_receive_communicator
{

public:
    zmq_receive_communicator() : receiver_socket ( static_zmq::context, sock_recv_type )
    {
        assert ( sock_recv_type == ZMQ_SUB );
        receive_buffer.rebuild ( MAX_PACKET_LENGTH );
    }

    ~zmq_receive_communicator()
    {
        receiver_socket.close();
    }

protected:
    /**
     * This function will block till the syncing phase is over, if the class was created with a syncing policy!
     * */
    bool init_full ( std::string owner_name, bool filter, std::string receiver_protocol, unsigned int expected_senders = 1, bool bind_receiver = false )
    {
        this->expected_senders = expected_senders;
        if ( bind_receiver )
        {
            receiver_socket.bind ( receiver_protocol.c_str() );
        }
        else
        {
            receiver_socket.connect ( receiver_protocol.c_str() );
        }

        if ( sock_recv_type == ZMQ_SUB )
        {
            receiver_socket.setsockopt ( ZMQ_SUBSCRIBE, "", 0 );
        }
        int temp = 0;
        receiver_socket.setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );

        results.resize ( expected_senders );
        this->owner_name = owner_name;
        return true;
    }

public:
    std::vector<receive_type> receive()
    {
        unsigned int subscribers = 0;
        receive_type packet;
        results.clear();
        while ( subscribers < expected_senders )
        {
            try
            {
                receiver_socket.recv ( &receive_buffer );
            }
            catch ( zmq::error_t ex )
            {
                if ( zmq_errno() == EINTR )
                    WARN ( "programma terminato", NULL );
                break;
            }
            
              char* receive = reinterpret_cast<char*> ( receive_buffer.data() );
	    //TODO(Mirko): codice brutto, migliorare
            std::cout<<receive<<std::endl;
	    std::string tmp;
	    std::istringstream iss(receive);
	    iss >> tmp; 
// 	    std::cout<<tmp<<std::endl;
//             std::istringstream receive_stream (tmp);
               // std::string ( receive, receive_buffer.size() ) );
            boost::archive::text_iarchive archive ( iss );
            archive >> packet;
            results.push_back ( packet );
            subscribers++;
        
          
        }
        return results;
    };


private:
    zmq::socket_t receiver_socket;
    zmq::message_t receive_buffer;
    std::vector <receive_type> results;
    std::string owner_name;
    unsigned int expected_senders;
};

#endif //ZMQ_COMMUNICATOR_HPP
