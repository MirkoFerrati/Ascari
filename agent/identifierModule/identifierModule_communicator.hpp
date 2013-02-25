//written by Alessandro Settimi

#include <../shared/communication/zmq_identifier_communicator.hpp>

#include <mutex>
#include <thread>
#include <typedefs.h>
#include <utility>

template <class send_type>
class identifierModule_communicator: public identifier_to_viewer_communicator<send_type>

{	
	send_type& data_send;
	std::string my_id;
	

public:

	identifierModule_communicator(send_type& data_send,std::string& my_id)
			:data_send(data_send), my_id(my_id)
	{
		this->init(my_id);
	}
	

	void send()
	{
		std::cout<<"dato inviato ";
		this->identifier_to_viewer_communicator<send_type>::send(data_send);
		
	}
	
	~identifierModule_communicator()
	{}

};