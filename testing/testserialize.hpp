#ifndef TESTSERIALIZE_HPP
#define TESTSERIALIZE_HPP


#include <vector>
#include <map>
#include <string>
#include <boost/concept_check.hpp>

#include "testClass.h"
#include <types/agent_sim_packet.h>
#include <types/world_sim_packet.h>


using namespace std;

class testSerialize:public testClass
{
public:
    void test()
    {
	std::string target="AGENTE1";
	
	
	std::map<std::string,double> bonus;
	bonus["DST"]=20;
	bonus["DIM"]=100;
	simulation_time t=10;
	agent_state_packet agente1;
	agent_state_packet agente2;
	
	agente1.identifier="AGENTE1";
	agente1.state[0]=1.0;
	agente1.state[1]=1.1;
	
	agente2.identifier="AGENTE2";
	agente2.state[0]=2.0;
	agente2.state[1]=2.1;
	
	objects_container tasks;	
	task_assignment_namespace::task app;
	
	app.id="T1";
	
	task_assignment_task* ta=new task_assignment_task(app);
	
	
	tasks.objects.push_back(ta);
	
	agent_sim_packet agent_packet(bonus,t,tasks);
	agent_packet.state_agents.internal_map[agente1.identifier]=&agente1;
	agent_packet.state_agents.internal_map[agente2.identifier]=&agente2;
	
	world_sim_packet world_packet;
	world_packet.object_list=tasks;
	world_packet.bonus_variables=agent_packet.bonus_variables;
	world_packet.state_agents.internal_map["AGENTE1"]=agente1;
	world_packet.state_agents.internal_map["AGENTE2"]=agente2;
	
	
	
        std::ostringstream archive_stream;
        boost::archive::text_oarchive archive ( archive_stream );
        archive << agent_packet;
        std::string tmp=target;
        tmp.append ( " " );
        tmp.append ( archive_stream.str() );
        
	std::cout<<tmp<<std::endl;
	
	
	
	
	agent_sim_packet_receiver sim_packet;
	//alle boost sembra che non piacciano le reference in ricezione
	

	std::string tmp_receive;
        std::istringstream iss ( tmp );
        iss >> tmp_receive;
	
	std::cout<<tmp_receive<<std::endl;
	boost::archive::text_iarchive archive_receive ( iss );
           
	archive_receive >> sim_packet;
	
	assert(sim_packet.bonus_variables["DST"]==20);
	assert(sim_packet.bonus_variables["DIM"]==100);
	assert(sim_packet.state_agents.internal_map["AGENTE1"]->state.at(0)==1.0);
	assert(sim_packet.state_agents.internal_map["AGENTE2"]->state.at(1)==2.1);

	


    }
};

#endif
