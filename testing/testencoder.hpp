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
       map<int,sub_event_value> topologies;
       map<int,sub_event_value> lambda;
       
       map<string,int > stateVariablesName_to_Index;
       stateVariablesName_to_Index.insert(make_pair<string,int>("x",0));
       stateVariablesName_to_Index.insert(make_pair<string,int>("y",1));
       stateVariablesName_to_Index.insert(make_pair<string,int>("z",2));
       agent_state state;
       state.insert(make_pair<int,double>(0,5));
       state.insert(make_pair<int,double>(1,2));
       state.insert(make_pair<int,double>(2,5));
       agent_state state_ith_agent;
       state_ith_agent.insert(make_pair<int,double>(0,3));
       state_ith_agent.insert(make_pair<int,double>(1,1));
       state_ith_agent.insert(make_pair<int,double>(2,3));
	map<string,int>bonusVariablesName_to_Index;
	bonusVariablesName_to_Index.insert(make_pair<string,int>("s",0));
	bonusVariablesName_to_Index.insert(make_pair<string,int>("t",1));
	map<int,double>  bonusVariables;
	bonusVariables.insert(make_pair<int,double>(0,1));
	bonusVariables.insert(make_pair<int,double>(1,0));	
	vector<string> topology_names;
	topology_names.push_back("t1");
	topology_names.push_back("t2");
	topology_names.push_back("t3");
	
	map<string,string>topology_expres;
	topology_expres.insert(make_pair<string,string>("t1","x-xi<0 or t>=0"));
	topology_expres.insert(make_pair<string,string>("t2","y-yi>0 and s<1 "));
	topology_expres.insert(make_pair<string,string>("t3","z+zi<3"));
	
	
	vector<string> lambda_names;
	lambda_names.push_back("l1");
	lambda_names.push_back("l2");
	
	map<string,string>lambda_expres;
	lambda_expres.insert(make_pair<string,string>("l1","x+y+s==8"));
	lambda_expres.insert(make_pair<string,string>("l2","x+y+t>10"));
	
	encoderDet E(topologies, lambda,state,state_ith_agent,stateVariablesName_to_Index, bonusVariables,bonusVariablesName_to_Index,
		      topology_expres, topology_names,lambda_expres, lambda_names);
        
	E.computeSubEvents();
	
	assert(abs(topologies.at(0)==_TRUE));
	assert(abs(topologies.at(1)==_FALSE));
	assert(abs(topologies.at(2)==_FALSE));
	
	assert(abs(lambda.at(0)==_TRUE));
	assert(abs(lambda.at(1)==_FALSE));
	
      //assert(abs(new_state[1]-(2+0.01*(3*10+3*2)))<0.0001);
	//assert(abs(new_state[2]-(10+0.01*(4*10+2*1+2)))<0.0001);
	}

};

#endif // TESTENCODER_HPP
