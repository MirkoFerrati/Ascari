//written by Alessandro Settimi

#ifndef TASK_ASSIGNMENT_ROUTER_H
#define TASK_ASSIGNMENT_ROUTER_H
#include <../shared/communication/zmq_task_assignment_communicator.hpp>

#include <mutex>
#include <thread>
#include <vector>
#include <map>
#include <string>
#include <typedefs.h>

class task_assignment_router_base
{
public:
	virtual void start_thread()=0;
	virtual ~task_assignment_router_base(){};
	virtual void init(unsigned int num_agent)=0;
};

template <typename datatype>
class task_assignment_router: public simulator_to_agent_ta_communicator<datatype,datatype>, public task_assignment_router_base
{		
  	std::vector<datatype> data;
//	std::mutex data_mutex;
	std::thread* sender_receiver;
	
	void loop(std::vector<datatype>& data)
	{
		std::vector<datatype> temp;
		
		while(!s_interrupted)
		{
			sleep(0);
			
			temp=this->receive();//blocking call
			
//			data_mutex.lock();
			
			data=temp;
			
			for (unsigned int i=0;i<temp.size();i++)
			{
				//std::cout<<">> RESEND: "<<data.at(i).agent_id<<std::endl;
				this->send(data.at(i));
			}
		      
//			data_mutex.unlock();
		}
	}

    public:

	task_assignment_router(unsigned int num_agent)
	{
		init(num_agent);
		this->start_thread();
	}
	
	void init(unsigned int num_agent)
	{
		simulator_to_agent_ta_communicator<datatype,datatype>::init(num_agent);
	}
	
	void start_thread()
	{
		sender_receiver=new std::thread(&task_assignment_router::loop,std::ref(*this),std::ref(data));
	}

	~task_assignment_router()
	{}
};

#endif // TASK_ASSIGNMENT_ROUTER_H
