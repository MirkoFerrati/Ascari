//written by Alessandro Settimi

#ifndef EMPTY_ROUTER_H
#define EMPTY_ROUTER_H
#include <../shared/communication/zmq_task_assignment_communicator.hpp>
#include <communication/zmq_empty_communicator.hpp>

#include <mutex>
#include <thread>
#include <vector>
#include <map>
#include <string>
 

class empty_router_base
{
public:
	virtual void start_thread()=0;
	virtual ~empty_router_base(){};
	virtual void init(unsigned int num_agent)=0;
};

template <typename datatype>
class empty_router: public simulator_to_agent_empty_communicator<datatype,datatype>, public empty_router_base
{		
  	std::vector<datatype> data;
	std::mutex data_mutex;
	std::thread* sender_receiver;
	
	void loop(std::vector<datatype>& data, std::mutex& data_mutex)
	{
		std::vector<datatype> temp;
		
		while(1)
		{
			sleep(0);
			
			temp=this->receive();//blocking call
			
			data_mutex.lock();
			
			data=temp;
			
			for (unsigned int i=0;i<temp.size();i++)
			{
				std::cout<<">> AGENT:"<<data[i].agent_id<<std::endl;
				this->send(data[i]);
			}
		      
			data_mutex.unlock();
		}
	}

    public:

	empty_router()
	{
		this->start_thread();
	}
	
	void init(unsigned int num_agent)
	{
		simulator_to_agent_empty_communicator<datatype,datatype>::init(num_agent);
	}
	
	void start_thread()
	{
		sender_receiver=new std::thread(&empty_router::loop,std::ref(*this),std::ref(data),std::ref(data_mutex));
	}

	~empty_router()
	{}
};

#endif // EMPTY_ROUTER_H
