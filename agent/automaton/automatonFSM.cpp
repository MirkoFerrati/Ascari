#include "automatonFSM.h"

using namespace std;

automatonFSM::automatonFSM(const transitionTable& table)
:automatonAbstract(table){}

forward_list<automaton_state> automatonFSM::getNextAutomatonState(const forward_list<automaton_state>& oldState, const map<transition,bool>& event)
{
	forward_list<automaton_state> result;
	result.push_front(oldState.front());
	std::map<transition,automaton_state>::iterator next_state;
	for (map<transition,bool>::const_iterator it=event.begin(); it!=event.end(); ++it)
	{ 
		if (it->second) { 
			if ( (next_state=table.internalTable.at(oldState.front()).find(it->first)) != table.internalTable.at(oldState.front()).end() ){
				result.front() = next_state->second;
				break;
			}
		}
	}
	return result;
}

