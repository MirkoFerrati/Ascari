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

  std::string my_leader; 
  std::string my_id;
  
  std::thread* receiver;

  void receive_loop(std::vector<formation_control_packet>& data_receive, std::mutex& data_mutex, std::string leader);

public:
  formation_control_communicator(std::string my_id, formation_control_packet* data_send, std::string my_leader);
  ~formation_control_communicator();
  
  void init(std::string agent_id);
  
  void start_threads();
 
  bool get_new_data(formation_control_packet* packet);
  void send();
};

#endif //ISAGENT