#ifndef ZMQ_LOCALIZATION_COMMUNICATOR_HPP
#define ZMQ_LOCALIZATION_COMMUNICATOR_HPP


#include <zmq.h>
#include <thread>
#include "zmq_communicator.hpp"
#include "types/agent_state_packet.h"
#include "streams_utils.h"
#define LOCALIZATION_TO_SIMULATOR CONFIG.getValue("LOCALIZATION_TO_SIMULATOR")

class zmq_localization_communicator_receiver: public zmq_receive_communicator<agent_state_packet_from_webcam,ZMQ_PULL>
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
	/*
	 * 0.1 soglia in salita e discesa
	 * -10% in discea
	 * raddoppio in salita
	 * inserire mittente
	 * mappa webcam-pesi
	 * 
	 */
agent_state getState(std::string agent_name){
      agent_state temp;
      agent_lock.lock();
      temp= agents.at(agent_name);
      agent_lock.unlock();
      return temp;
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
      agent_state_packet_from_webcam temp=(this->receive().front());
      if (!webcams.count(temp.webcam_id))
	webcams[temp.webcam_id];
      if (!webcams.at(temp.webcam_id).count(temp.data.identifier))
	webcams[temp.webcam_id][temp.data.identifier];
      auto& value=weights[temp.webcam_id][temp.data.identifier];
      if (value>0.1)
      {
	value=2*value;
	if (value>1)
	  value=1;
      }
      else
	value=0.1;
      for (auto weight:weights)
      {
	if (weight.first!=temp.webcam_id)
	                   weight.second.at(temp.data.identifier)=weight.second.at(temp.data.identifier)*0.9;
	if (weight.second.at(temp.data.identifier)<=0.1)
	                   weight.second.at(temp.data.identifier)=0;
      }
      webcams.at(temp.webcam_id).at(temp.data.identifier)=temp.data.state;
      agent_state medium_state=temp.data.state;
      for (auto coordinate:medium_state)
      {	
	coordinate.second=0;
	int num_webcams=0;
	for(auto single_webcam=webcams.begin();single_webcam!=webcams.end();++single_webcam)
	{ 
	  num_webcams++;
	  coordinate.second+=single_webcam->second.at(temp.data.identifier).at(coordinate.first);
	} 
	coordinate.second=coordinate.second/num_webcams;
      }
      agent_lock.lock();
      agents[temp.data.identifier]=medium_state;
      //std::cout<<"ricevuto agente "<<temp.identifier<<" con stato "<<temp.state<<std::endl;
      agent_lock.unlock();
    }
  }
  bool exiting;
  std::thread* receiver_loop;
  std::mutex agent_lock;
  std::map<std::string,std::map<std::string , double>> weights;
  std::map<std::string, std::map<std::string,agent_state>> webcams;
  std::map<std::string,agent_state>  agents;
  
};

class zmq_localization_communicator_sender : public zmq_send_communicator<agent_state_packet_from_webcam,ZMQ_PUSH>
{
public:
void init(std::string owner_name)
	{
		this->init_full(owner_name,LOCALIZATION_TO_SIMULATOR,false);
	}
};

#endif // ZMQ_LOCALIZATION_COMMUNICATOR_HPP

