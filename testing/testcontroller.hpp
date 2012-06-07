#ifndef TESTCONTROLLER_HPP
#define TESTCONTROLLER_HPP

#include <../agent/controller/controller.h>


#include <vector>
#include <map>
#include <string>
#include "typedefs.h"
#include "testClass.h"

using namespace std;

class testController:public testClass
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
        map<int,string> mappa_id_espressioni;
        mappa_id_espressioni.insert(make_pair<int,string>(0,"5x+2y"));
        mappa_id_espressioni.insert(make_pair<int,string>(1,"3z"));
        agent_state stato;
        stato.insert(make_pair<int,double>(0,1));
        stato.insert(make_pair<int,double>(1,2));
        stato.insert(make_pair<int,double>(2,10));
        control_command controllo;
        controllo[0]=0;
        controllo[1]=0;
        controller c(stato,nomi_variabili,mappa_id_espressioni,nomi_controllo,controllo);
        c.computeControl();
        assert(controllo[0]==5*1+2*2);
        assert(controllo[1]==3*10);
        stato[0]=5;
        c.computeControl();
        assert(controllo[0]==5*5+2*2);
    }

};

#endif // TESTCONTROLLER_HPP
