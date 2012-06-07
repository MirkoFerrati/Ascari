#include "automatonFSM.h"

automatonFSM::automatonFSM(const transitionTable& table, const string& name): automatonAbstract(table,name)
{}

vector<automaton_state> automatonFSM::getNextAutomatonState(const vector<automaton_state>& oldState, const vector<transition>& transitions)
{
	vector<automaton_state> result;
	result[0] = oldState[0];
	for (vector<transition>::const_iterator it=transitions.begin(); it!=transitions.end(); it++)
	{
		if (table.internalTable.at(oldState[0]).count(*it)) {
			result[0] = (table.internalTable.at(oldState[0]).at(*it));
			break;
		}
	}
	return result;
}

