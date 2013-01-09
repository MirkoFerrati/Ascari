#ifndef ZMQ_WORLD_SNIFFER_HPP
#define ZMQ_WORLD_SNIFFER_HPP
#include "world_sniffer_abstract.h"
#include "../shared/communication/zmq_full_communicator.hpp"
#include <mutex>
#include <thread>

template <typename receive_type> 
class zmq_world_sniffer: 
public world_sniffer_abstract, public zmq_sniffer_simulator_communicator<receive_type>
{

public: 
	zmq_world_sniffer(receive_type& data,std::shared_ptr<std::mutex>& data_mutex):data(data),data_mutex(data_mutex)
	{
		this->init("viewer");
	}
	;
	void start_receiving()
	{
		receiver=new std::thread(&zmq_world_sniffer::receive_loop,std::ref(*this),std::ref(data),std::ref(data_mutex));
	};

private:
receive_type& data;
std::shared_ptr<std::mutex>& data_mutex;
std::thread* receiver;
void receive_loop(receive_type& data, std::shared_ptr<std::mutex> data_mutex)
{
	receive_type temp;
	while(1)
	{
		sleep(0);
		temp=this->receive().front();
		data_mutex->lock();
		data=temp;
		data_mutex->unlock();
	}
}
};




#endif //ZMQ_WORLD_SNIFFER_HPP