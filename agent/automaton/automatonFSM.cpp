#include "automatonFSM.h"

using namespace std;

automatonFSM::automatonFSM(const transitionTable& table)
:automatonAbstract(table){}

forward_list<automaton_state> automatonFSM::getNextAutomatonState(const forward_list<automaton_state>& oldState, const map<transition,bool>& event)
{
	forward_list<automaton_state> result;
	result.push_front(oldState.front());
	for (map<transition,bool>::const_iterator it=event.begin(); it!=event.end(); ++it)
	{ 
		if (it->second) { 
			if (table.internalTable.at(oldState.front()).count(it->first)) {
				result.front() = (table.internalTable.at(oldState.front()).at(it->first));
				break;
			}
		}
	}
	return result;
}

