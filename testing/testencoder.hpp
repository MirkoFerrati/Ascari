#ifndef TESTENCODER_HPP
#define TESTENCODER_HPP

#include <../agent/encoder/encoderDet.h>


#include <vector>
#include <map>
#include <string>
#include "typedefs.h"
#include "testClass.h"

using namespace std;

class testEncoder:public testClass
{
public:
    void test() {
       
       
       map<string,int > stateVariablesName_to_Index;
       stateVariablesName_to_Index.insert(make_pair<string,int>("x",0));
       stateVariablesName_to_Index.insert(make_pair<string,int>("y",1));
       stateVariablesName_to_Index.insert(make_pair<string,int>("z",2));
       agent_state state;
       state.insert(make_pair<int,double>(0,5));
       state.insert(make_pair<int,double>(1,2));
       state.insert(make_pair<int,double>(2,5));
        map< string, agent_state_packet > state_other_agents;
	
	agent_state &state_ith_agent= state_other_agents["pippo"].state;
	
       state_ith_agent.insert(make_pair<int,double>(0,3));
       state_ith_agent.insert(make_pair<int,double>(1,1));
       state_ith_agent.insert(make_pair<int,double>(2,3));
	map<string,int>bonusVariablesName_to_Index;
	bonusVariablesName_to_Index.insert(make_pair<string,int>("s",0));
	bonusVariablesName_to_Index.insert(make_pair<string,int>("t",1));
	map<int,double>  bonusVariables;
	bonusVariables.insert(make_pair<int,double>(0,1));
	bonusVariables.insert(make_pair<int,double>(1,0));	
	
	
	index_map sub_event_to_index;
	
	sub_event_to_index.insert(make_pair("t1",0));
	sub_event_to_index.insert(make_pair("t2",1));
	sub_event_to_index.insert(make_pair("t3",2));
	sub_event_to_index.insert(make_pair("l1",3));
	sub_event_to_index.insert(make_pair("l2",4));
	
	
	map< int, sub_event_value > sub_events;
	
	sub_events[0]=_FALSE;
	sub_events[1]=_FALSE;
	sub_events[2]=_FALSE;
	sub_events[3]=_FALSE;
	sub_events[4]=_FALSE;
	
	map<string,string>topology_expres;
	topology_expres.insert(make_pair<string,string>("t1","x-xi<0 or t>=0"));
	topology_expres.insert(make_pair<string,string>("t2","y-yi>0 and s<1 "));
	topology_expres.insert(make_pair<string,string>("t3","z+zi<3"));
	
	
	map<string,string>lambda_expres;
	lambda_expres.insert(make_pair<string,string>("l1","x+y+s==8"));
	lambda_expres.insert(make_pair<string,string>("l2","x+y+t>10"));
	
	
	
	encoderDet E(sub_events,"pluto", state, stateVariablesName_to_Index, state_other_agents, bonusVariables, 
		       bonusVariablesName_to_Index, topology_expres, sub_event_to_index, lambda_expres); 
	E.computeSubEvents();
	
	assert(sub_events.at(sub_event_to_index.at("t1"))==_TRUE);
	assert(sub_events.at(sub_event_to_index.at("t2"))==_FALSE);
	assert(sub_events.at(sub_event_to_index.at("t3"))==_FALSE);
	
	assert(sub_events.at(sub_event_to_index.at("l1"))==_TRUE);
	assert(sub_events.at(sub_event_to_index.at("l2"))==_FALSE);
	
      //assert(abs(new_state[1]-(2+0.01*(3*10+3*2)))<0.0001);
	//assert(abs(new_state[2]-(10+0.01*(4*10+2*1+2)))<0.0001);
	}

};

#endif // TESTENCODER_HPP
