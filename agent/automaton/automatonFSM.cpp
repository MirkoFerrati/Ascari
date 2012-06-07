#include "automatonFSM.h"

automatonFSM::automatonFSM(const transitionTable& table): automatonAbstract(table)
{

}
vector< automaton_state > automatonFSM::getNextAutomatonState(const std::vector< automaton_state >& oldStates, const std::vector< transition >& transitions)
{
	vector< automaton_state> result;
	return result;
}
