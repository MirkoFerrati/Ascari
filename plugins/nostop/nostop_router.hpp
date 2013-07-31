#ifndef NOSTOP_ROUTER_HPP
#define NOSTOP_ROUTER_HPP
#ifdef ISSIMULATOR
#include <../shared/communication/zmq_nostop_communicator.hpp>

#include <mutex>
#include <thread>
#include <vector>
#include <map>
#include <string>

namespace NoStop
{
	//////////////////////////////////////////////////////////////////////////
	/// Communicator from Simulator to Agent
	template <typename datatype>
	class Router: public simulator_to_agent_nostop_communicator<datatype,datatype>
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

		Router(unsigned int num_agent)
		{
			init(num_agent);
			this->start_thread();
		}

		void init(unsigned int num_agent)
		{
			simulator_to_agent_nostop_communicator<datatype,datatype>::init(num_agent);
		}

		void start_thread()
		{
			sender_receiver=new std::thread(&Router::loop,std::ref(*this),std::ref(data));
		}

		~Router()
		{}
	};
}


#endif //ISSIMULATOR
#endif // TASK_ASSIGNMENT_ROUTER_H
