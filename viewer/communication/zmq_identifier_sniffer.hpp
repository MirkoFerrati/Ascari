#ifndef ZMQ_WORLD_SNIFFER_HPP
#define ZMQ_WORLD_SNIFFER_HPP
#include "../shared/communication/zmq_identifier_communicator.hpp"
#include "world_sniffer_abstract.h"
#include <mutex>
#include <thread>

template <typename receive_type> 
class zmq_identifier_sniffer: public world_sniffer_abstract,
public viewer_from_identifier_sniffer<monitor_packet>
{

public: 
	zmq_identifier_sniffer(receive_type& data,std::shared_ptr<std::mutex>& data_mutex):data(data),data_mutex(data_mutex)
	{
		this->init();
	}
	;
	void start_receiving()
	{
		receiver=new std::thread(&zmq_identifier_sniffer::receive_loop,std::ref(*this),std::ref(data),std::ref(data_mutex));
	};

private:
receive_type& data;
std::shared_ptr<std::mutex>& data_mutex;
std::thread* receiver;
void receive_loop(receive_type& data, std::shared_ptr<std::mutex> data_mutex)
{
	std::vector<receive_type> temp;
	while(!s_interrupted)
	{
		sleep(0);
		temp=this->receive();//.front();//blocking call
		if (temp.size()>0)
		{
		data_mutex->lock();
		data=temp.front();
		data_mutex->unlock();
		}
	}
}
};
#endif //ZMQ_WORLD_SNIFFER_HPP
