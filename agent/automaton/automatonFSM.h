#ifndef AUTOMATONFSM_H
#define AUTOMATONFSM_H

#include <automatonabstract.h>

using namespace std;

class automatonFSM:public automatonAbstract
{
public:
  automatonFSM(const transitionTable& table);
  virtual vector<automaton_state> getNextAutomatonState(vector<automaton_state>const& oldStates, vector<transition>const& transitions);

private:
  string name;
  transitionTable* tableFSM;
  
};

#endif // AUTOMATONFSM_H