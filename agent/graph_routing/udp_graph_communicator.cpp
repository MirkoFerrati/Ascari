#include "udp_graph_communicator.h"
#include <types/graph_informations.h>

Udp_graph_communicator::Udp_graph_communicator(std::mutex& mutex,graph_packet* tp,boost::asio::io_service& io_service,std::string identifier)
	:identifier(identifier),mutex(mutex),tp(tp),socket_(io_service),sender(io_service,boost::asio::ip::address::from_string(MULTICAST_ADDRESS),SIMULATOR_GRAPH_PORT)
		,listen_endpoint_(boost::asio::ip::address::from_string(SOCKET_BINDING), AGENT_GRAPH_PORT),_io_service(io_service)
{
    mutex_is_mine=false;
	printDebug=false;
	should_run=false;
	std::fill_n(inbound_data_,MAX_PACKET_LENGTH,0);
    socket_.open(listen_endpoint_.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.bind(listen_endpoint_);

    // Join the multicast group.
    socket_.set_option(
        boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(MULTICAST_ADDRESS)));
}


	void Udp_graph_communicator::service_thread(void) {
	while (should_run) {
		_io_service.run();
		_io_service.reset();
		std::cout<<"ricevuti i pacchetti"<<std::endl;
		usleep(10000);
	}
}

Udp_graph_communicator::~Udp_graph_communicator()
{
	should_run=false;
	_io_service.stop();
	if (t)
		t->join();
}


/*!
 * Per chi avesse dubbi, usare map::operator= implica distruggere e ricreare ogni volta ogni elemento della mappa
 * Invece impostare i valori con un ciclo for crea solo i nuovi elementi, sovrascrive solo i valori di quelli
 * già esistenti, non distrugge i vecchi
 */
void Udp_graph_communicator::send(graph_informations infos, bool printDebug )
{
    
	if (printDebug)
		std::cout<<"sto inviando una comunicazione"<<std::endl;
    output_map_tp.clear();
    mutex.lock();
    output_map_tp[identifier]=infos;
    mutex.unlock();
	if (printDebug)
		std::cout<<output_map_tp<<std::endl;
    sender.send(output_map_tp);
//     sleep(1);
}

void Udp_graph_communicator::startReceive(bool printDebug)
{
	should_run=true;
	this->printDebug=printDebug;
	t=thread_ptr(new std::thread(boost::bind(&Udp_graph_communicator::service_thread,this)));
    socket_.async_receive_from(
        boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), listen_endpoint_,
        boost::bind(&Udp_graph_communicator::handle_receive_from, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Udp_graph_communicator::handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
{
    if (!error)
    {
        try
        {
            if (printDebug) 
				std::cout<<"ricezione: "<<std::endl;

            std::string archive_data(&inbound_data_[header_length], MAX_PACKET_LENGTH-header_length);
            std::istringstream archive_stream(archive_data);
            boost::archive::text_iarchive archive(archive_stream);
            archive >> input_map_tp;
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
        if (printDebug) 
			std::cout<<input_map_tp<<std::endl;
//         if (!mutex_is_mine)
// 		{
        mutex.lock();
// 			mutex_is_mine=true;
// 		}

        //Copiamo tutto, sarà compito del sender assicurarsi di non mandare robaccia
        for (graph_packet::iterator it=input_map_tp.begin();it!=input_map_tp.end();++it)
        {
			//se non ho l'informazione oppure ce l'ho ma e' vecchia la cambio
            if (!tp->count(it->first) || tp->at(it->first).timestamp<it->second.timestamp) 
            {
                (*tp)[it->first]=it->second;
            }
        }
// 		if (!socket_.available())
// 		{
// 			mutex_is_mine=false;
        mutex.unlock();
// 		}

        socket_.async_receive_from(
            boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), listen_endpoint_,
            boost::bind(&Udp_graph_communicator::handle_receive_from, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
}
