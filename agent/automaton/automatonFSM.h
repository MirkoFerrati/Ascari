#ifndef AUTOMATONFSM_H
#define AUTOMATONFSM_H

#include "automatonabstract.h"


class automatonFSM:public automatonAbstract
{
	
public:
  automatonFSM(const transitionTable& table);
  std::forward_list<automaton_state> getNextAutomatonState(const std::forward_list< automaton_state >& oldState, const std::map< transition, events::value >& event);

private:
  //string name;
  //transitionTable* tableFSM;
	
};

#endif // AUTOMATONFSM_H