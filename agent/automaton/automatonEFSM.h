#ifndef AUTOMATONEFSM_H
#define AUTOMATONEFSM_H

#include "automatonabstract.h"

class automatonEFSM:public automatonAbstract
{
public:
    automatonEFSM(const transitionTable& table);
	std::vector< automaton_state > getNextAutomatonState(const std::vector< automaton_state >& oldStates, const std::vector< transition >& transitions);
	
private:

	
};

#endif // AUTOMATONEFSM_H