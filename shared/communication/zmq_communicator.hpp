#ifndef ZMQ_COMMUNICATOR_HPP
#define ZMQ_COMMUNICATOR_HPP
#include <zmq.hpp>
#include <vector>

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
#include "../../types/agent_simulator_handshake_packet.h"
#include "../define.h"
#include <types.h>

enum Synctypes
{
    NO_SYNC,
    WAIT_SYNC,
    ASK_SYNC
};






template<typename receive_type, int sock_recv_type>
class zmq_receive_communicator
{
public:
    zmq_receive_communicator()
    {
        receiver_socket=std::unique_ptr<zmq::socket_t> (new zmq::socket_t(*static_zmq::context, sock_recv_type ));
        assert ( sock_recv_type == ZMQ_PULL || sock_recv_type == ZMQ_SUB );
        clientsNamed=false;
        initialized = false;
        receive_buffer.rebuild ( MAX_PACKET_LENGTH );
        int temp = 0;
        receiver_socket->setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );

    }

    ~zmq_receive_communicator()
    {
        if (receiver_socket)
        {
            receiver_socket->close();
            delete(receiver_socket.release());
        }
//       std::cout<<"chiamato distruttore di zmq_communicator"<<std::endl;

    }

protected:

    std::list<std::string> clients;

    void setClientsName ( const std::list<std::string>& clients )
    {
        clientsNamed = true;
        this->clients = clients;
    }


    bool init_full ( std::string owner_name,bool filter, std::string receiver_protocol,
                     unsigned int expected_senders = 1, bool bind_receiver = false )
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
            receiver_socket->bind ( receiver_protocol.c_str() );
        }
        else
        {
            receiver_socket->connect ( receiver_protocol.c_str() );
        }
        if ( sock_recv_type == ZMQ_SUB )
        {
            if ( filter )
                receiver_socket->setsockopt ( ZMQ_SUBSCRIBE, owner_name.data(), strlen ( owner_name.data() ) );
            else
                receiver_socket->setsockopt ( ZMQ_SUBSCRIBE, "", 0 );
        }

        results.resize ( expected_senders );
        this->owner_name = owner_name;
        initialized = true;
        return true;
    }


    bool internal_receive(zmq::message_t* buffer,int flags=0)
    {

#ifdef ZMQDEBUG
        bool fail = !check_for_unique_call.try_lock();
        if ( fail )
        {
            ERR ( "receive e' gia' in esecuzione,probabilmente ci sono due thread concorrenti che chiamano la stessa receive", NULL );
        }
#endif //ZMQDEBUG
// 	      std::cout<<"mi sto per bloccare su una receive"<<std::endl;
        bool rc=receiver_socket->recv ( buffer,flags );
// 			      std::cout<<"sbloccato da una receive"<<std::endl;

        if ( !rc )
        {
            if (flags==ZMQ_NOBLOCK)
                return false;
            else
                ERR ( "brutte cose, questa scritta non dovrebbe mai comparire",NULL );
        }
#ifdef ZMQDEBUG
        check_for_unique_call.unlock();
#endif //ZMQDEBUG
        return rc;

    }

    void internal_translate(std::vector<std::string>& received_strings)
    {
        receive_type packet;
        results.clear();
for (auto receive:received_strings)
        {   
	  std::string tmp;
            std::istringstream iss ( receive );
            iss >> tmp;
            boost::archive::text_iarchive archive ( iss );
            try {
                archive >> packet;
            }
            catch (const char* ex)
            {
                ERR("errore durante la deserializzazione del pacchetto %s",ex);
            }
            results.push_back ( packet );
        }
    }


public:



    receive_type receive_last_one()
    {
        if ( !initialized )
        {
            ERR ( "receive chiamata senza avere inizializzato il communicator", NULL );
            throw "receive chiamata senza avere inizializzato il communicator";
        }

        received_strings.clear();
        received_strings.resize(1);
        results.clear();
        bool end=false;
        bool found=false;
        while(!end && !s_interrupted)
        {
            try
            {
                bool rc=internal_receive(&receive_buffer,ZMQ_NOBLOCK);
                if(rc)
                {
                    found=true;
                    received_strings[0]=( reinterpret_cast<char*> ( receive_buffer.data() ));
                }
                else if (!found)
                {
                    bool rc=internal_receive(&receive_buffer);
                    if(rc)
                    {
			found=true;
                        received_strings[0]=( reinterpret_cast<char*> ( receive_buffer.data() ));
                    }
                }
                else
                {
                    end=true;
                }
            }
            catch ( zmq::error_t& ex )
            {
                if ( zmq_errno () == ETERM )
                {
                    WARN ( "programma terminato o qualcosa del genere",NULL );
                    receiver_socket->close();
                    delete(receiver_socket.release());
                    return results.at(0);//TODO solve undefined behavior
                }
            }
        }
        internal_translate(received_strings);
        return results.at(0);
    }

#ifdef ZMQDEBUG
    std::mutex check_for_unique_call;

#endif //ZMQDEBUG

    std::vector<receive_type>& receive ( int flags = 0 )
    {
        if ( !initialized )
        {
            ERR ( "receive chiamata senza avere inizializzato il communicator", NULL );
            throw "receive chiamata senza avere inizializzato il communicator";
        }

        unsigned int subscribers = 0;

        received_strings.clear();
        while ( subscribers < expected_senders )
        {
            try
            {
                internal_receive(&receive_buffer,flags);
            }
            catch ( zmq::error_t& ex )
            {
                if ( zmq_errno () == ETERM )
                {
                    WARN ( "programma terminato o qualcosa del genere",NULL );
                    receiver_socket->close();
                    delete(receiver_socket.release());
                    return results;
                }
            }
            received_strings.push_back( reinterpret_cast<char*> ( receive_buffer.data() ));
            subscribers++;
        }

        internal_translate(received_strings);
        return results;

    };

private:
    std::unique_ptr<zmq::socket_t>  receiver_socket;
    zmq::message_t receive_buffer;
    std::vector <receive_type> results;
    std::vector<std::string> received_strings;
    std::string owner_name;
    unsigned int expected_senders;
    bool initialized;
protected:
    bool clientsNamed;
};

template <typename send_type, int sock_send_type>
class zmq_send_communicator
{
public:
    zmq_send_communicator()// : sender_socket ( static_zmq::context, sock_send_type )
    {
        sender_socket=std::unique_ptr<zmq::socket_t> (new zmq::socket_t(*static_zmq::context, sock_send_type ));

        assert ( sock_send_type == ZMQ_PUSH || sock_send_type == ZMQ_PUB );
        initialized = false;
        send_buffer.rebuild ( MAX_PACKET_LENGTH );
        int temp = 0;
        sender_socket->setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );
    }

    ~zmq_send_communicator()
    {

    }

protected:
    bool init_full ( std::string owner_name, std::string sender_protocol , bool bind_sender = true )
    {
        if ( bind_sender )
        {
            sender_socket->bind ( sender_protocol.c_str() );
        }
        else
        {
            sender_socket->connect ( sender_protocol.c_str() );
        }
        this->owner_name = owner_name;
        initialized = true;
        return true;
    }

public:

#ifdef ZMQDEBUG
    std::mutex check_for_unique_call;

#endif //ZMQDEBUG

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
        tmp.append ( " " );
        tmp.append ( archive_stream.str() );
        send_buffer.rebuild ( tmp.length() +1 );
        memcpy ( send_buffer.data(), tmp.c_str(),tmp.length() +1 );
        std::string temp=reinterpret_cast<char*> ( send_buffer.data() );
        //std::cout<<temp<<std::endl;
        sender_socket->send ( send_buffer );
    };


private:
    std::unique_ptr<zmq::socket_t> sender_socket;
    zmq::message_t send_buffer;
    std::string owner_name;
    bool initialized;
};


/**
 *
 * Receiver and sender are (optionally) synchronized
 * \param infos sent is not modified, data received are returned in a std::vector
 * Everything is copied in a buffer, sometimes more than once
 * TODO(Mirko): implement zero-copy
 */
template <typename receive_type, typename send_type, int sock_send_type, int sock_recv_type, Synctypes sync = NO_SYNC>
class zmq_communicator: public zmq_receive_communicator<receive_type,sock_recv_type>,public zmq_send_communicator<send_type,sock_send_type>
{

public:
    zmq_communicator()
    {
        assert ( sock_recv_type == ZMQ_PULL || sock_recv_type == ZMQ_SUB );
        assert ( sock_send_type == ZMQ_PUSH || sock_send_type == ZMQ_PUB );
        initialized = false;
        sync_socket = 0;
        int temp = 0;
        if ( sync == WAIT_SYNC )
        {
            sync_socket = new zmq::socket_t ( *static_zmq::context, ZMQ_REP );
            sync_socket->setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );
        }
        else if ( sync == ASK_SYNC )
        {
            //sync_socket = new zmq::socket_t ( static_zmq::context, ZMQ_REQ );
            //sync_socket->setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );
        }
    }

    ~zmq_communicator()
    {
        if ( sync_socket )
        {
            sync_socket->close();
            delete sync_socket;
        }
    }

protected:

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
            catch ( zmq::error_t& ex )
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
            //bool clientsNamed=zmq_receive_communicator<receive_type,sock_recv_type>::clientsNamed;
            //auto clients=zmq_receive_communicator<receive_type,sock_recv_type>::clients;
            if ( this->clientsNamed )
            {
                auto client = std::find ( this->clients.begin(), this->clients.end(), name );
                if ( client != this->clients.end() )
                {
                    this->clients.erase ( client );
                    rejected = false;
                }
                else
                {
                    rejected=true;
                }
            }
            agent_simulator_handshake_packet infos;
            if ( !rejected || !this->clientsNamed )
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


    bool askSync ( std::string sync_protocol )
    {
        /**   TODO(Mirko): Publisher opens PUB socket and starts sending "Hello" messages (not data).
                 Subscribers connect SUB socket and when they receive a Hello message they tell the publisher via a REQ/REP socket pair.
                 When the publisher has had all the necessary confirmations, it starts to send real data.
               */

        std::cout << owner_name << " connecting to server...";
        std::cout.flush();
        agent_simulator_handshake_packet infos;
        bool exit=false;

        int temp=0;


        while ( !exit && !s_interrupted)
        {
            sync_socket = new zmq::socket_t ( *static_zmq::context, ZMQ_REQ );
            sync_socket->setsockopt ( ZMQ_LINGER, &temp, sizeof ( temp ) );
            sync_socket->connect ( !sync_protocol.compare ( "" ) ? SYNC_PROTOCOL : sync_protocol.c_str() );
            std::cout  << "."<<std::flush;
            zmq::message_t message ( owner_name.size() );
	    usleep(250000); //HACK:between connect and send we need some time, or we delete the socket too early
            memcpy ( message.data(), owner_name.data(), owner_name.size() );
            bool rc = sync_socket->send ( message );
	    usleep(250000); //HACK:between send and receive we need some time, or we delete the socket too early
            if (!rc) assert ( rc ); //When we compile without assertion, do not warn about unused variable
            message.rebuild ( MAX_PACKET_LENGTH );
            try
            {
                zmq::pollitem_t items[] = { { *sync_socket, 0, ZMQ_POLLIN, 0 } };
                zmq::poll ( &items[0], 1, 1 * 1000 );
                //  If we got a reply, process it
                if ( items[0].revents & ZMQ_POLLIN )
                {
                    std::cout<<"ricevuto qualcosa"<<std::endl;
                    bool rc = sync_socket->recv ( &message );
                    if ( rc )
                    {
                        char* receive = reinterpret_cast<char*> ( message.data() );
                        //std::cout<<receive<<std::endl;
                        std::istringstream receive_stream (
                            std::string ( receive, message.size() ) );
                        boost::archive::text_iarchive archive ( receive_stream );
                        try
                        {
                            archive >> infos;
                            exit=true;
                        }
                        catch ( std::exception &ex )
                        {
                            ERR ( "error while parsing sync response from server %s",ex.what() );
                            throw ex;
                        }
                    }
                }
                else
                {
                    sync_socket->close();
                    delete sync_socket;
                    usleep ( 250000 );
                    continue;
                }

            }
            catch ( zmq::error_t ex )
            {
                WARN ( "%s",ex.what() );
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
        this->owner_name=owner_name;
        this->expected_senders=expected_senders;
        zmq_receive_communicator<receive_type,sock_recv_type>::init_full ( owner_name,filter,receiver_protocol,expected_senders,bind_receiver );
        zmq_send_communicator<send_type,sock_send_type>::init_full ( owner_name,sender_protocol,bind_sender );
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
            //sync_socket->connect ( !sync_protocol.compare ( "" ) ? SYNC_PROTOCOL : sync_protocol.c_str() );

            if ( !askSync ( sync_protocol ) )
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

private:
    zmq::socket_t  *sync_socket;
    std::string owner_name;
    unsigned int expected_senders;
    bool initialized;

};

#endif //ZMQ_COMMUNICATOR_HPP
