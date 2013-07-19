#include "fake_simulator.h"
#include "zmq_agent_communicator.h"

void fake_simulator::create_communicator(int type, const Parsed_World& world)
{
        std::list<std::string> clients;
        for ( auto ag:world.agents )
        {
          if (ag.simulated)
          {
            clients.push_back ( ag.name );
            std::cout<<ag.name<<std::endl;
          }
        }
        communicator=new zmq_agent_communicator ( clients.size(),clients );      
}

 
void fake_simulator::initialize(const Parsed_World& world)
{

}

void fake_simulator::setPeriod(int period)
{

}

void fake_simulator::start_sim(int count)
{

}
