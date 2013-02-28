#ifndef AUTOMATONEFSM_H
#define AUTOMATONEFSM_H

#include "automatonabstract.h"
#include "typedefs.h"
class automatonEFSM:public automatonAbstract
{
public:
    automatonEFSM(const transitionTable& table);
    std::forward_list< automaton_state > getNextAutomatonState (const std::forward_list< automaton_state >& oldStates, const std::map< transition, Events >& event);	
private:

	
};

#endif // AUTOMATONEFSM_H