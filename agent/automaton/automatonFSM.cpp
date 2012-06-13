#include "automatonFSM.h"

automatonFSM::automatonFSM(const transitionTable& table)
:automatonAbstract(table){}

vector<automaton_state> automatonFSM::getNextAutomatonState(const vector<automaton_state>& oldState, const map<transition,bool>& event)
{
	vector<automaton_state> result;
	result.push_back(oldState.at(0));
	for (map<transition,bool>::const_iterator it=event.begin(); it!=event.end(); it++)
	{ 
		if (it->second) { 
			if (table.internalTable.at(oldState.at(0)).count(it->first)) {
				result[0] = (table.internalTable.at(oldState.at(0)).at(it->first));
				break;
			}
		}
	}
	return result;
}

