#ifdef ISAGENT

#include "formation_control_communicator.h"



formation_control_communicator::formation_control_communicator(std::string my_id, formation_control_packet* data_send, std::string my_leader)
: my_id(my_id), 
  data_send(*data_send), 
  my_leader(my_leader)
{
  this->init(this->my_id);
  this->start_threads();
}


formation_control_communicator::~formation_control_communicator(){}


void formation_control_communicator::receive_loop(std::vector<formation_control_packet>& data_receive, std::mutex& data_mutex, std::string leader)
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
      if(!temp[i].agent_state.identifier.compare(my_leader))
      data_receive.push_back(temp[i]);
    }
    data_mutex.unlock();
  }
}
  
  
void formation_control_communicator::init(std::string agent_id)
{
  agent_to_simulator_formation_control_communicator<formation_control_packet, formation_control_packet>::init(agent_id);
}


void formation_control_communicator::start_threads()
{
  this->receiver = new std::thread(&formation_control_communicator::receive_loop, std::ref(*this), std::ref(this->data_receive), std::ref(this->data_receive_mutex), std::ref(this->my_leader));
}


bool formation_control_communicator::get_new_data(formation_control_packet* packet)
{
  formation_control_packet tmp;

  if(this->data_receive.empty())
    return false;
  
  this->data_receive_mutex.lock();
  tmp = this->data_receive.back();
  this->data_receive.clear();
  this->data_receive_mutex.unlock();
  
  *packet = tmp;
  return true;
}


void formation_control_communicator::send()
{
  this->agent_to_simulator_formation_control_communicator<formation_control_packet, formation_control_packet>::send(data_send);
}

    
#endif //ISAGENT