#ifndef FORMATION_CONTROL_ROUTER_H
#define FORMATION_CONTROL_ROUTER_H

#ifdef ISSIMULATOR

#include "../../shared/communication/zmq_formation_control_communicator.hpp"
#include "formation_control_types.h"

#include <mutex>
#include <thread>
#include <vector>
#include <map>
#include <string>
 

class formation_control_router
: public simulator_to_agent_formation_control_communicator<formation_control_packet, formation_control_packet>
{
  std::vector<formation_control_packet> data;
  std::thread* sender_receiver;
  
  void loop(std::vector<formation_control_packet>& data)
  {
    while(!s_interrupted)
    {
      sleep(0);
      
      data = this->receive();//blocking call
      
      for (unsigned int i = 0; i < data.size(); i++)
      {
	this->send(data.at(i));
      }
    }
  }

public:
  formation_control_router(unsigned int num_agent)
  {
    init(num_agent);
    this->start_thread();
  }

  void init(unsigned int num_agent)
  {
    simulator_to_agent_formation_control_communicator<formation_control_packet, formation_control_packet>::init(num_agent);
  }

  void start_thread()
  {
    this->sender_receiver = new std::thread(&formation_control_router::loop, std::ref(*this), std::ref(data));
  }

  ~formation_control_router(){}
};

#endif //ISSIMULATOR

#endif //FORMATION_CONTROL_ROUTER_H
