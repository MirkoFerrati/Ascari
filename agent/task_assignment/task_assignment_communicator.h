//written by Alessandro Settimi

#include <../shared/communication/zmq_task_assignment_communicator.hpp>

#include <mutex>
#include <thread>
#include <typedefs.h>
#include <utility>


class task_assignment_communicator_base
{
public:
  	virtual void start_threads()=0;
	virtual ~task_assignment_communicator_base(){};
	virtual void init(std::string agent_id)=0;
	virtual void* get_data()=0;
	virtual void send()=0;
};

template <class receive_type, class send_type>
class task_assignment_communicator: public agent_to_simulator_ta_communicator<receive_type,send_type>, public task_assignment_communicator_base
{	
	std::shared_ptr<std::mutex>& data_receive_mutex;
	std::vector<receive_type> data_receive;
	send_type& data_send;
	
	std::thread* receiver;
	
	const bool& converge;
	unsigned int neighbour;
	std::string my_id;
	bool& fresh_data;
	std::map< std::string, bool > received_from_agents;
	
	void receive_loop(std::vector<receive_type>& data_receive,std::shared_ptr<std::mutex>& ptr_data_mutex,const bool& converge,unsigned int neighbour,std::string my_id,bool& fresh_data)
	{
	      std::vector<receive_type> temp;

	      while(!converge && !s_interrupted)
	      {
		      sleep(0);

		      while (!fresh_data && !s_interrupted) //il dato precedente non è stato ancora usato
		      {
			    temp=this->receive();//blocking call

			    ptr_data_mutex->lock();

			    for (unsigned int i=0;i<temp.size();i++)
			    {
				if(temp[i].agent_id!=my_id && !(received_from_agents.count(temp[i].agent_id)))
				{
				    received_from_agents.insert(make_pair(temp[i].agent_id,true));
				    data_receive.push_back(temp[i]);
				    std::cout<<"Ricevo da "<<temp[i].agent_id<<std::endl;
				}
			    }

			    if (data_receive.size()<neighbour)
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

	task_assignment_communicator(std::shared_ptr<std::mutex>& data_receive_mutex,send_type* data_send,
				     const bool& converge,unsigned int neighbour,std::string my_id,bool& fresh_data)
	:data_receive_mutex(data_receive_mutex),data_send(*data_send),converge(converge),neighbour(neighbour),my_id(my_id),fresh_data(fresh_data)
	{
		this->init(my_id);
		this->start_threads();
	}
	
	void init(std::string agent_id)
	{
		agent_to_simulator_ta_communicator<receive_type,send_type>::init(agent_id);
	}

	void start_threads()
	{
		receiver=new std::thread(&task_assignment_communicator::receive_loop,std::ref(*this),std::ref(data_receive),std::ref(data_receive_mutex),std::ref(converge),std::ref(neighbour),std::ref(my_id),std::ref(fresh_data));
	}
	
	void* get_data()
	{
		return (void*) &data_receive;
	}
	
	void send()
	{
		this->agent_to_simulator_ta_communicator<receive_type,send_type>::send(data_send);
	}
	
	~task_assignment_communicator()
	{

	}

};