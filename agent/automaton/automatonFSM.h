#ifndef AUTOMATONFSM_H
#define AUTOMATONFSM_H

#include "automatonabstract.h"

using namespace std;


class automatonFSM:public automatonAbstract
{
	
public:
  automatonFSM(const transitionTable& table);
  vector<automaton_state> getNextAutomatonState(vector<automaton_state>const& oldStates, map<transition,bool>const& event);

private:
  //string name;
  //transitionTable* tableFSM;
	
};

#endif // AUTOMATONFSM_H