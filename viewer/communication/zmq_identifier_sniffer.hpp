
#include "../shared/communication/zmq_identifier_communicator.hpp"
#include "world_sniffer_abstract.h"
#include <mutex>
#include <thread>
#include <map>
#include <vector>

class zmq_identifier_sniffer: public world_sniffer_abstract,
public viewer_from_identifier_sniffer<monitor_packet>
{

public: 
	zmq_identifier_sniffer(std::map<std::string,monitor_packet>& data,std::shared_ptr<std::mutex>& data_mutex):data(data),data_mutex(data_mutex)
	{
		this->init();
		receiver=0;
		isrunning=false;
	}
	;
	void start_receiving()
	{
	  isrunning=true;
		receiver=new std::thread(&zmq_identifier_sniffer::receive_loop,std::ref(*this),std::ref(data),std::ref(data_mutex),std::ref(isrunning));
	};

	void stop_receiving()
	{
// 	        std::cout<<"chiamato stop_receiving di identifier sniffer"<<std::endl;

	 isrunning=false; 
	}
	
    ~zmq_identifier_sniffer()
    {
//      if (receiver)
//	delete(receiver);
if (receiver)
  receiver->join();

//       std::cout<<"thread receiver joinato, chiamato distruttore di identifier sniffer"<<std::endl;
    };
	
private:
std::map<std::string,monitor_packet>& data;
std::shared_ptr<std::mutex>& data_mutex;
std::thread* receiver;
bool isrunning;
void receive_loop(std::map<std::string,monitor_packet>& data, std::shared_ptr<std::mutex> data_mutex,bool & isrunning)
{
	std::vector<monitor_packet> temp;
	while(!s_interrupted && isrunning)
	{
		sleep(0);
		try
		{
		temp=this->receive();//.front();//blocking call
		}
		catch (std::exception & ex)
		{
		  WARN("errore durante la receive: %s",ex.what()); 
		}
// 		std::cout<<"ricevuto un pacchetto"<<std::endl;
		if (!temp.empty())
		{
		data_mutex->lock();
		data[temp.front().id]=temp.front();
		data_mutex->unlock();
		}
	}
// 	      std::cout<<"fine loop di identifier sniffer thread"<<std::endl;

}
};

