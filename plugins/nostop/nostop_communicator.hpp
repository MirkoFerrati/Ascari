#ifndef NOSTOP_COMMUNICATOR_HPP
#define NOSTOP_COMMUNICATOR_HPP
#ifdef ISAGENT
#include <../shared/communication/zmq_nostop_communicator.hpp>
#include "nostop_packet.hpp"

#include <mutex>
#include <thread>
#include <utility>

namespace NoStop
{
  	class Communicator: public agent_to_simulator_nostop_communicator<Coverage_packet,Coverage_packet>
	{	
		std::shared_ptr<std::mutex>& data_receive_mutex;
		std::vector<Coverage_packet> data_receive;
		Coverage_packet& data_send;

		std::thread* receiver;

		unsigned int neighbour;
		std::string my_id;
		bool& fresh_data; // if not all data are available, do not use packet
		std::map< std::string, bool > received_from_agents;

		void receive_loop(
			std::vector<Coverage_packet>& data_receive,
			std::shared_ptr<std::mutex>& ptr_data_mutex,
			unsigned int neighbour,
			std::string my_id,
			bool& fresh_data)
		{
			std::vector<Coverage_packet> temp;

			while(!s_interrupted)
			{
				sleep(0);

				while (!fresh_data && !s_interrupted) //il dato precedente non è stato ancora usato
				{
					temp=this->receive();//blocking call

					ptr_data_mutex->lock();

					data_receive.clear();

					std::cout<<"Ricevo da ";

					for (unsigned int i=0;i<temp.size();i++)
					{
						if(temp[i].m_agent_id.str()!=my_id )
						{
							if (!(received_from_agents.count(temp[i].m_agent_id.str())))
								received_from_agents.insert(std::make_pair(temp[i].m_agent_id.str(),true));
							data_receive.push_back(temp[i]);
							std::cout<<temp[i].m_agent_id.str()<<',';
						}
					}

					std::cout<<std::endl;

					if (received_from_agents.size()<neighbour)
					{
						fresh_data=false;
					}
					else
					{
						fresh_data=true;
						received_from_agents.clear();
						std::cout<<std::endl;
					}

					ptr_data_mutex->unlock();
				}
			}
		}

	public:

		Communicator(
			std::shared_ptr<std::mutex>& data_receive_mutex,
			Coverage_packet* data_send,
			unsigned int neighbour,
			std::string my_id,
			bool& fresh_data)
			:	data_receive_mutex(data_receive_mutex),
			data_send(*data_send),
			neighbour(neighbour),
			my_id(my_id),
			fresh_data(fresh_data)
		{
			this->init(my_id);
			this->start_threads();
		}

		void init(std::string agent_id)
		{
			agent_to_simulator_nostop_communicator<Coverage_packet,Coverage_packet>::init(agent_id);
		}

		void start_threads()
		{
			receiver=new std::thread(
				&Communicator::receive_loop,
				std::ref(*this),
				std::ref(data_receive),
				std::ref(data_receive_mutex),
				std::ref(neighbour),
				std::ref(my_id),
				std::ref(fresh_data) );
		}

		void* get_data()
		{
			return (void*) &data_receive;
		}

		void send()
		{
			this->agent_to_simulator_nostop_communicator<Coverage_packet,Coverage_packet>::send(data_send);
		}

		~Communicator()
		{

		}
		protected:
		void ref(std::shared_ptr< std::mutex > data_receive_mutex);
	};
}

#endif //ISAGENT
#endif //NOSTOP_COMMUNICATOR_HPP