//written by Alessandro Settimi

#include <../shared/communication/zmq_empty_communicator.hpp>

#include <mutex>
#include <thread>
#include <typedefs.h>
#include <utility>

class empty_communicator_base
{
public:
  	virtual void start_thread()=0;
	virtual ~empty_communicator_base(){};
	virtual void init(std::string agent_id)=0;
	virtual void send()=0;
};

template <class receive_type, class send_type>
class empty_communicator: public agent_empty_simulator_ta_communicator<receive_type,send_type>, public empty_communicator_base
{	
	std::vector<receive_type>& data_receive;
	const send_type& data_send;
	
	std::shared_ptr<std::mutex>& data_mutex;
	std::thread* receiver;

	std::string my_id;
	
	void receive_loop(std::vector<receive_type>& data_receive, std::shared_ptr<std::mutex> data_mutex,std::string my_id)
	{
	      std::vector<receive_type> temp;

	      while(1)
	      {
		      sleep(0);

		        temp=this->receive();//blocking call

			    data_mutex->lock();

			    for (unsigned int i=0;i<temp.size();i++)
			    {
				if(temp[i].agent_id!=my_id)
				{
				    data_receive.push_back(temp[i]);
				    std::cout<<"Ricevo da "<<temp[i].agent_id<<std::endl;
				}
			    }

			    std::cout<<std::endl;
			    

			    data_mutex->unlock();
		      
		      
	      }
	      
	}

public:

	empty_communicator(std::vector<receive_type>& data_receive,const receive_type& data_send,
								 std::shared_ptr<std::mutex>& data_mutex,std::string my_id)
			:data_receive(data_receive),data_send(data_send),data_mutex(data_mutex),
				     my_id(my_id)
	{
		this->init(my_id);
		this->start_thread();
	}
	
	void init(std::string agent_id)
	{
		agent_to_simulator_ta_communicator<receive_type,send_type>::init(agent_id);
	}

	void start_thread()
	{
		receiver=new std::thread(&empty_communicator::receive_loop,std::ref(*this),std::ref(data_receive),std::ref(data_mutex),std::ref(my_id));
	}
	
	void send()
	{
		//NOTE: we will not use mutex here, because serialization inside send is done in the same thread of the caller.
		this->send(data_send);
	}
	
	~task_assignment_communicator()
	{}

};