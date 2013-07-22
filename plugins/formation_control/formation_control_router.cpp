#ifdef ISSIMULATOR

#include "formation_control_router.h" 


formation_control_router::formation_control_router(unsigned int num_agent)
  : num_agent(num_agent)
{
  init(num_agent);
//   this->start_thread();
}


formation_control_router::~formation_control_router(){}


void formation_control_router::start_thread()
{
  this->sender_receiver = new std::thread(&formation_control_router::loop, std::ref(*this), std::ref(data));
}

  
void formation_control_router::init(unsigned int num_agent)
{
  simulator_to_agent_formation_control_communicator<formation_control_packet, formation_control_packet>::init(num_agent);
}


void formation_control_router::loop(std::vector< formation_control_packet >& data)
{
  while(!s_interrupted)
  {
    sleep(0);

    // DA MODIFICARE
    data = this->receive();//blocking call
    
    for (unsigned int i = 0; i < data.size(); i++)
      this->send(data.at(i));  
  }
}


void formation_control_router::sync_messages()
{
  std::vector<formation_control_packet> temp;
  
  this->received_data.clear();
  
  while(this->received_data.size() != this->num_agent)
  {
    temp = this->receive();
    this->received_data.insert(this->received_data.end(), temp.begin(), temp.end());
    temp.clear();
  }
  
  for (unsigned int i = 0; i < this->received_data.size(); i++)
    this->send(this->received_data.at(i));
}



#endif //ISSIMULATOR