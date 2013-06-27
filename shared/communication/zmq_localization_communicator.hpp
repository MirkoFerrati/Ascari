#ifndef ZMQ_LOCALIZATION_COMMUNICATOR_HPP
#define ZMQ_LOCALIZATION_COMMUNICATOR_HPP


#include <zmq.h>
#include <thread>
#include "zmq_communicator.hpp"
#include "types/agent_state_packet.h"

#define LOCALIZATION_TO_SIMULATOR CONFIG.getValue("LOCALIZATION_TO_SIMULATOR")

class zmq_localization_communicator_receiver: public zmq_receive_communicator<agent_state_packet,ZMQ_PULL>
{
  public:
void init(std::string owner_name)
	{
		this->init_full(owner_name,false,LOCALIZATION_TO_SIMULATOR,1,true);
		exiting=false;
		receiver_loop=new std::thread(&zmq_localization_communicator_receiver::loop,std::ref(*this),std::ref(agent_lock),std::ref(agents),std::ref(exiting));
	}
	
void stop()
{
  exiting=true;
}
	
agent_state getState(std::string agent_name){
      agent_lock.lock();
      return agents.at(agent_name);
      agent_lock.unlock();
};	
    void addAgent(std::string name, agent_state  state)
    {
	agent_lock.lock();
	agents[name]=state;
	agent_lock.unlock();
    }
	
private:
  
  void loop(std::mutex& agent_lock,std::map<std::string,agent_state>& agents,bool& exiting)  
  {
    while(!exiting)
    {
      agent_lock.lock();
      auto temp=this->receive().front();
      agents[temp.identifier]=temp.state;
      agent_lock.unlock();
    }
  }
  bool exiting;
  std::thread* receiver_loop;
  std::mutex agent_lock;
  std::map<std::string,agent_state> agents;
  
};

class zmq_localization_communicator_sender : public zmq_send_communicator<agent_state_packet,ZMQ_PUSH>
{
public:
void init(std::string owner_name)
	{
		this->init_full(owner_name,LOCALIZATION_TO_SIMULATOR,false);
	}
};

#endif // ZMQ_LOCALIZATION_COMMUNICATOR_HPP

