//written by Alessandro Settimi

#include <../shared/communication/zmq_identifier_communicator.hpp>

#include <mutex>
#include <thread>
#include <typedefs.h>
#include <utility>

template <class receive_type, class send_type>
class identifierModule_communicator: public identifier_to_viewer_communicator<send_type>

{	receive_type& data_receive;
	send_type& data_send;
	std::string my_id;
	

public:

	identifierModule_communicator(send_type& data_send,receive_type& data_receive,std::string& my_id)
			:data_send(data_send),data_receive(data_receive), my_id(my_id)
	{
		
	}
	

	void receive_loop(std::vector<receive_type>& data_receive, std::shared_ptr<std::mutex> data_mutex,std::string my_id)
	{
	      
	}
	
	
	void send()
	{
		this->identifier_to_viewer_communicator<send_type>::send(data_send);
	}
	
	~identifierModule_communicator()
	{}

};