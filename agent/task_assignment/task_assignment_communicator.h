//written by Alessandro Settimi

#include <../shared/communication/zmq_task_assignment_communicator.hpp>

#include <mutex>
#include <thread>
#include <typedefs.h>
#include <utility>

class task_assignment_communicator_base
{
public:
  	virtual void start_thread()=0;
	virtual ~task_assignment_communicator_base(){};
	virtual void init(std::string agent_id)=0;
};

template <class receive_type, class send_type>
class task_assignment_communicator: public agent_to_simulator_ta_communicator<receive_type,send_type>
{	
	std::vector<receive_type>& data_receive;
	
	std::shared_ptr<std::mutex>& data_mutex;
	std::thread* receiver;
	const bool& converge;
	unsigned int neighbour;
	std::string my_id;
	bool& fresh_data;
	std::map< std::string, bool > received_from_agents;
	
	void receive_loop(std::vector<receive_type>& data_receive, std::shared_ptr<std::mutex> data_mutex,const bool& converge,unsigned int neighbour,std::string my_id,bool& fresh_data)
	{
	      std::vector<receive_type> temp;

	      while(!converge)
	      {
		      sleep(0);

		      while (!fresh_data) //il dato precedente non è stato ancora usato
		      {
			    temp=this->receive();//blocking call

			    data_mutex->lock();

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

			    data_mutex->unlock();
		      }
		      
	      }
	      
	}

public:

	task_assignment_communicator(std::vector<receive_type>& data_receive,std::shared_ptr<std::mutex>& data_mutex,const bool& converge,unsigned int neighbour,std::string my_id,bool& fresh_data):data_receive(data_receive),data_mutex(data_mutex),converge(converge),neighbour(neighbour),my_id(my_id),fresh_data(fresh_data)
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
		receiver=new std::thread(&task_assignment_communicator::receive_loop,std::ref(*this),std::ref(data_receive),std::ref(data_mutex),std::ref(converge),std::ref(neighbour),std::ref(my_id),std::ref(fresh_data));
	}

	~task_assignment_communicator()
	{}

};