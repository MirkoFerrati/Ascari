#ifndef ASYNC_ZMQ_RECEIVER
#define ASYNC_ZMQ_RECEIVER

#include <thread>
#include <mutex>
#include "../shared/communication/zmq_full_communicator.hpp"
#include "world_sniffer_abstract.h"
#include <functional>
template <typename receive_type>
class zmq_world_sniffer: public world_sniffer_abstract,
public zmq_sniffer_simulator_communicator<receive_type>
{
public:
  zmq_world_sniffer(receive_type& data_buffer, std::shared_ptr<std::mutex> data_mutex):data_buffer(data_buffer),data_mutex(data_mutex)
  {
  
  this->init("viewer");
  
}
  void start_receiving()
  {
  receiver=new std::thread(&zmq_world_sniffer<receive_type>::receive_thread,std::ref(*this),std::ref(data_buffer),data_mutex); 
}


  
private:

  void receive_thread (receive_type& data_buffer, std::shared_ptr<std::mutex> mutex)
{
  receive_type temp;
    while(1)
    {
     sleep(0); 
     temp=this->receive().front();//blocking call
     mutex->lock();
     data_buffer=temp;
     mutex->unlock();
    }
}
  receive_type& data_buffer;
  std::shared_ptr<std::mutex> data_mutex;
  std::thread* receiver;

};







#endif //ASYNC_ZMQ_RECEIVER