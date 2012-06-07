#ifndef TESTDYNAMIC_HPP
#define TESTDYNAMIC_HPP

#include <../shared/dynamic.h>


#include <vector>
#include <map>
#include <string>
#include "typedefs.h"

using namespace std;

class testDynamic
{
public:
    void test() {
        vector<string> nomi_variabili;
        nomi_variabili.push_back("x");
        nomi_variabili.push_back("y");
        nomi_variabili.push_back("z");
        vector<string> nomi_controllo;
        nomi_controllo.push_back("v");
        nomi_controllo.push_back("w");
        map<string,string> mappa_id_espressioni;
        mappa_id_espressioni.insert(make_pair<string,string>("x","5x+2y+3v"));
        mappa_id_espressioni.insert(make_pair<string,string>("y","3z+3w"));
        mappa_id_espressioni.insert(make_pair<string,string>("z","3z+2x+z+y"));
        agent_state stato;
        stato.insert(make_pair<int,double>(0,1));
        stato.insert(make_pair<int,double>(1,2));
        stato.insert(make_pair<int,double>(2,10));
        control_command controllo;
        controllo[0]=1;
        controllo[1]=2;
        dynamic d(stato,controllo,mappa_id_espressioni,nomi_variabili,nomi_controllo);
	
	agent_state new_state=d.getNextState();
	
	assert(stato[0]==1+0.01*(5*1+2*2+3*1));
        assert(stato[1]==2+0.01*(3*10+3*2));
	assert(stato[2]==10+0.01*(4*10+2*1+2));
	}

};

#endif // TESTDYNAMIC_HPP
