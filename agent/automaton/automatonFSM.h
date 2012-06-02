#ifndef AUTOMATONFSM_H
#define AUTOMATONFSM_H

#include <automatonabstract.h>



class automatonFSM:public automatonAbstract
{
	using namespace std;

public:
  automatonFSM(const transitionTable& table);
  vector<automaton_state> getNextAutomatonState(vector<automaton_state>const& oldStates, vector<transition>const& transitions);

private:
  string name;
  transitionTable* tableFSM;
  
};

#endif // AUTOMATONFSM_H