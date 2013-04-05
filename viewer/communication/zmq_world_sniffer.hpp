
#include "world_sniffer_abstract.h"
#include "../shared/communication/zmq_full_communicator.hpp"
#include <mutex>
#include <thread>
#include <vector>

template <typename receive_type> 
class zmq_world_sniffer: 
public world_sniffer_abstract, public zmq_sniffer_simulator_communicator<receive_type>
{

public: 
	zmq_world_sniffer(receive_type& data,std::shared_ptr<std::mutex>& data_mutex):data(data),data_mutex(data_mutex)
	{
		this->init("viewer");
		receiver=0;
		is_running=false;
	}
	;
	void start_receiving()
	{
		is_running=true;
		receiver=new std::thread(&zmq_world_sniffer::receive_loop,std::ref(*this),std::ref(data),std::ref(data_mutex),std::ref(is_running));
	};

    void stop_receiving()
    {
//       	        std::cout<<"chiamato stop_receiving di world sniffer"<<std::endl;

      is_running=false;
    }
	
	~zmq_world_sniffer(){
// 	        	        std::cout<<"chiamato distruttore di world sniffer"<<std::endl;

		if (receiver)
		{
			receiver->join();
		}
	};
	
private:
receive_type& data;
bool is_running;
std::shared_ptr<std::mutex>& data_mutex;
std::thread* receiver;
void receive_loop(receive_type& data, std::shared_ptr<std::mutex>& data_mutex,bool& is_running)
{
	std::vector<receive_type> temp;
	while(!s_interrupted && is_running)
	{
		sleep(0);
		temp=this->receive();//.front();//blocking call
// 				std::cout<<"ricevuto un pacchetto"<<std::endl;

		if (!temp.empty())
		{
		data_mutex->lock();
		data=temp.front();
		data_mutex->unlock();
		}
	}
// 		      std::cout<<"fine loop di world sniffer thread"<<std::endl;

}
};


