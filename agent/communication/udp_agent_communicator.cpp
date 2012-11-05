#include "udp_agent_communicator.h"

udp_agent_communicator::udp_agent_communicator(boost::signals2::mutex& mutex,topology_packet* tp,boost::asio::io_service& io_service)
:mutex(mutex),tp(tp),socket_(io_service),sender(io_service,boost::asio::ip::address::from_string(MULTICAST_ADDRESS),SIMULATOR_ROUTE_PORT)
,listen_endpoint_(boost::asio::ip::address::from_string("0.0.0.0"), AGENT_ROUTE_PORT)
{
	std::fill_n(inbound_data_,MAX_PACKET_LENGTH,0);
	mutex_is_mine=false;
    socket_.open(listen_endpoint_.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.bind(listen_endpoint_);
	printDebug=false;
    // Join the multicast group.
    socket_.set_option(
        boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(MULTICAST_ADDRESS)));
}

/*!
 * Per chi avesse dubbi, usare map::operator= implica distruggere e ricreare ogni volta ogni elemento della mappa
 * Invece impostare i valori con un ciclo for crea solo i nuovi elementi, sovrascrive solo i valori di quelli
 * già esistenti, non distrugge i vecchi
 */
void udp_agent_communicator::send(bool printDebug)
{
//TODO in teoria un sender asincrono dovrebbe andare, ma devo costruire la struttura dati pulita
    mutex.lock();
	if (printDebug)
		std::cout<<"sto inviando una comunicazione"<<std::endl;
    for (std::map<agent_name,behavior_topology>::iterator it=tp->data.begin();it!=tp->data.end();++it)
    {
        for (behavior_topology::iterator itt=it->second.begin();itt!=it->second.end();++itt)
        {
            for (topology_values::iterator ittt=itt->second.begin();ittt!=itt->second.end();++ittt)
            {
                if (ittt->second!=-1)
                    output_map_tp.data[it->first][itt->first][ittt->first]=ittt->second;
				if (printDebug)
					std::cout<<"["<<it->first<<" "<<itt->first<<" "<<ittt->first<<"="<<ittt->second<<"]";
            }
        }
    }
    if (printDebug)
		std::cout<<std::endl;
    mutex.unlock();
	sender.send(output_map_tp);
// 	sleep(1);
}

void udp_agent_communicator::startReceive(bool printDebug)
{
	this->printDebug=printDebug;
    socket_.async_receive_from(
        boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), listen_endpoint_,
        boost::bind(&udp_agent_communicator::handle_receive_from, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void udp_agent_communicator::handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
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
		{
			for (std::map<agent_name,behavior_topology>::iterator it=input_map_tp.data.begin();it!=input_map_tp.data.end();++it)
			{
				for (behavior_topology::iterator itt=it->second.begin();itt!=it->second.end();++itt)
				{
					for (topology_values::iterator ittt=itt->second.begin();ittt!=itt->second.end();++ittt)
					{
						std::cout<<"-->"<<it->first<<" "<<itt->first<<" "<<ittt->first<<" "<<ittt->second<<",";
					}
				}
			}
			std::cout<<std::endl;
		}
//         if (!mutex_is_mine)
// 		{
        mutex.lock();
// 			mutex_is_mine=true;
// 		}

        //Copiamo tutto, sarà compito del sender assicurarsi di non mandare robaccia
        for (std::map<agent_name,behavior_topology>::iterator it=input_map_tp.data.begin();it!=input_map_tp.data.end();++it)
        {
            for (behavior_topology::iterator itt=it->second.begin();itt!=it->second.end();++itt)
            {
                for (topology_values::iterator ittt=itt->second.begin();ittt!=itt->second.end();++ittt)
                {
                    tp->data[it->first][itt->first][ittt->first]=ittt->second;
                }
            }
        }

// 		if (!socket_.available())
// 		{
// 			mutex_is_mine=false;
        mutex.unlock();
// 		}

        socket_.async_receive_from(
            boost::asio::buffer(inbound_data_, MAX_PACKET_LENGTH), listen_endpoint_,
            boost::bind(&udp_agent_communicator::handle_receive_from, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
}
