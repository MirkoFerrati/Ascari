
#include <zmq.hpp>




template <class receive_type,int expected_senders,class send_type, int sock_send_type,int sock_recv_type>
class zmq_communicator{
	
public:
	zmq_communicator();
	bool init();
	receive_type receive();
	send_type send();
		
private:
	
	
};