#ifdef ISAGENT

#include "../../shared/communication/zmq_formation_control_communicator.hpp"
#include "formation_control_types.h"

#include <mutex>
#include <thread> 
#include <utility>


class formation_control_communicator
: public agent_to_simulator_formation_control_communicator<formation_control_packet, formation_control_packet>
{
  std::mutex data_receive_mutex;
  std::vector<formation_control_packet> data_receive;
  formation_control_packet& data_send;

  std::string leader; 
  std::string my_id;
  
  std::thread* receiver;

  void receive_loop(std::vector<formation_control_packet>& data_receive, std::mutex& data_mutex, std::string leader)
  {
    std::vector<formation_control_packet> temp;

    while(!s_interrupted)
    {
      sleep(0);

      temp = this->receive(); //blocking call
      
      data_mutex.lock();
 
      data_receive.clear();

      for(unsigned int i = 0; i < temp.size(); i++)
      {
	if(!temp[i].agent_state.identifier.compare(leader))
	  data_receive.push_back(temp[i]);
      }
       
      data_mutex.unlock();
    }
  }
  
public:
  formation_control_communicator(std::string my_id, formation_control_packet* data_send, std::string leader)
    : my_id(my_id), data_send(*data_send), leader(leader)
    {
      this->init(this->my_id);
      this->start_threads();
    }

    void init(std::string agent_id)
    {
      agent_to_simulator_formation_control_communicator<formation_control_packet, formation_control_packet>::init(agent_id);
    }

    void start_threads()
    {
      this->receiver = new std::thread(&formation_control_communicator::receive_loop, std::ref(*this), std::ref(data_receive), std::ref(data_receive_mutex),std::ref(leader));
    }

    formation_control_packet get_data()
    {
      formation_control_packet tmp;
      
      this->data_receive_mutex.lock();
      
      tmp = this->data_receive.at(this->data_receive.size()-1);

      this->data_receive_mutex.unlock();
      
      return tmp;
    }

    void send()
    {
      this->agent_to_simulator_formation_control_communicator<formation_control_packet, formation_control_packet>::send(data_send);
    }

    ~formation_control_communicator(){}
};

#endif //ISAGENT