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
    for (unsigned int i=0; i<ag.size();i++) {
        agents_name_to_index.insert(make_pair(ag.at(i).name,i));
        agent_state_packet agent_packet;
        control_command_packet command_packet;
        agent_packet.identifier=ag.at(i).name;
        command_packet.identifier=ag.at(i).name;
        index_map states_to_index_tmp;
        index_map commands_to_index_tmp;

        for (unsigned int j=0; j<ag.at(i).state.size();j++)
        {
            agent_packet.state.insert(make_pair(j,ag.at(i).initial_states.at(ag.at(i).state.at(j))));
            states_to_index_tmp.insert(make_pair(ag.at(i).state.at(j),j));
        }

        for (unsigned int j=0; j<ag.at(i).inputs.size();j++)
        {
            command_packet.command.insert(make_pair(j,0));
            commands_to_index_tmp.insert(make_pair(ag.at(i).inputs.at(j),j));

        }
        states_index.internal_map.insert(make_pair(ag.at(i).name,agent_packet));
        agent_states_to_index.push_back(states_to_index_tmp);
        commands.push_back(command_packet);
        agent_commands_to_index.push_back(commands_to_index_tmp);

        dynamic *d= new dynamic(states_index.internal_map.at(ag.at(i).name).state, commands.at(agents_name_to_index.at(ag.at(i).name)).command,
                                ag.at(i).expressions, ag.at(i).state,ag.at(i).inputs);

        dynamic_module.push_back(d);


    }


//     for (map<std::string,agent_state_packet>::iterator iter=states_index.internal_map.begin(); iter!=states_index.internal_map.end();iter++)
//     {
//         cout<<iter->first<<endl;
//         for (map<int,double>::iterator iiter=iter->second.state.begin();iiter!=iter->second.state.end();iiter++) {
//             //cout<< iiter->first<<"->"<<iiter->second<<endl;
//         }
//         for (map<string,int>::iterator iiter=agent_states_to_index.at(agent_name_to_index.at(iter->first)).begin();iiter!=agent_states_to_index.at(agent_name_to_index.at(iter->first)).end();iiter++) {
//             cout<< iiter->first<<"->"<<iter->second.state.at(iiter->second)<<endl;
//         }
//     }
}

void simulator::main_loop()
{
    try {
		int i=0;
        while (1) {
			i++;
//             communicator->send_broadcast(time++);
            communicator->send_broadcast(states_index);
			cout<<"inviato pacchetto con gli stati"<<endl;
            
            agent_state state_tmp;
            for (index_map::const_iterator iter=agents_name_to_index.begin(); iter!=agents_name_to_index.end();iter++) {
                state_tmp=dynamic_module.at(iter->second)->getNextState();
                for (agent_state::const_iterator iiter=state_tmp.begin();iiter!=state_tmp.end();iiter++) {
                    states_index.internal_map.at(iter->first).state.at(iiter->first)=iiter->second; //metto il nuovo stato nel posto giusto senza copiare i vettori

                }
            }
			//sleep(1);
            vector<control_command_packet> temp=communicator->receive_control_commands();
            cout<<"ricevuto pacchetto con i controlli"<<endl;
            for (unsigned i=0; i< temp.size();i++) {

                for (map<int,double>::iterator it=commands.at(agents_name_to_index.at(temp.at(i).identifier)).command.begin(); it!=commands.at(agents_name_to_index.at(temp.at(i).identifier)).command.end();it++) {

                    it->second=temp.at(i).command.at(it->first);

                }

            }
		if (states_index.internal_map.at("AGENTE1").state.at(0)>10)
				break;
        }
    }
    catch (const char* e)
    {
        ERR(e);
    }
}



simulator::~simulator()
{
    delete communicator;
	for (unsigned int i=0;i<dynamic_module.size();i++)
		delete dynamic_module[i];
}

void simulator::start_sim()
{
    time=0;
    //communicator->send_broadcast(time);
    main_loop();
}
