#include "simulator.h"
#include <udp_agent_communicator.h>
#include "typedefs.h"
#include "logog.hpp"

using namespace std;

void simulator::create_communicator(int communicator_type)
{
    if (communicator_type==1)
    {
        communicator=new udp_agent_communicator();
    }
}

void simulator::initialize(const vector<Parsed_Agent>& ag)
{ 
for(int i=0; i<ag.size();i++){
  agent_name_to_index.insert(make_pair<string,int>(ag.at(i).name,i));
  agent_state_packet agent_packet;
  control_command_packet command_packet;
  agent_packet.identifier=ag.at(i).name;
  command_packet.identifier=ag.at(i).name;
  index_map states_to_index_tmp;
  index_map commands_to_index_tmp;
  
  for (int j=0; j<ag.at(i).state.size();j++)
  {
    agent_packet.state.insert(make_pair<int,double>(j,0));
    states_to_index_tmp.insert(make_pair<std::string,int>(ag.at(i).state.at(j),j));
   }
  
  for (int j=0; j<ag.at(i).inputs.size();j++)
  {
    command_packet.command.insert(make_pair<int,double>(j,0));
    commands_to_index_tmp.insert(make_pair<std::string,int>(ag.at(i).inputs.at(j),j));
    
  }
    states_index.internal_map.insert(make_pair<string,agent_state_packet>(ag.at(i).name,agent_packet));
    agent_states_to_index.push_back(states_to_index_tmp);
    commands[i]=command_packet;
    agent_commands_to_index.push_back(commands_to_index_tmp);
  
}

for(map<std::string,agent_state_packet>::iterator iter=states_index.internal_map.begin(); iter!=states_index.internal_map.end();iter++)
{
cout<<iter->first<<endl;
for (map<int,double>::iterator iiter=iter->second.state.begin();iiter!=iter->second.state.end();iiter++){
  //cout<< iiter->first<<"->"<<iiter->second<<endl;
}
for (map<string,int>::iterator iiter=agent_states_to_index.at(agent_name_to_index.at(iter->first)).begin();iiter!=agent_states_to_index.at(agent_name_to_index.at(iter->first)).end();iiter++){
  cout<< iiter->first<<"->"<<iter->second.state.at(iiter->second)<<endl;
}
}
}

void simulator::main_loop()
{
    try {
        while (1) {
            communicator->send_broadcast(time++);
            //communicator->send_broadcast(states_index);
            dynamic_module.evolve();
			sleep(1);
			//TODO: we don't need to copy, we need a function that overwrites only values inside the same memory!!
			commands=communicator->receive_control_commands();
        }
    }
    catch (const char* e)
    {
        ERR(e);
    }
}


simulator::simulator(int argc, char** argv)
{
	for (int i=1;i<argc;i++)
	{
		identifiers.push_back(argv[i]);
		std::cout<<identifiers[i-1]<<std::endl;
	}
}

simulator::~simulator()
{
	delete communicator;
}

void simulator::start_sim()
{
    time=0;
    //communicator->send_broadcast(time);
    main_loop();
}
