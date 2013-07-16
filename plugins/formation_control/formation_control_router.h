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
  std::vector<formation_control_packet> received_data;
  std::thread* sender_receiver;
  int num_agent;
  
  void loop(std::vector<formation_control_packet>& data);
  
public:
  formation_control_router(unsigned int num_agent);
  ~formation_control_router();
  
  void init(unsigned int num_agent);

  void start_thread();
  
  void sync_messages();
};

#endif //ISSIMULATOR

#endif //FORMATION_CONTROL_ROUTER_H
