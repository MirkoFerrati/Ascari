#include "automatonEFSM.h"

using namespace std;

automatonEFSM::automatonEFSM (const transitionTable& table) : automatonAbstract (table)
{

}



<<<<<<< HEAD
forward_list< automaton_state > automatonEFSM::getNextAutomatonState (const forward_list< automaton_state >& oldStates, const std::map< transition, bool >& event)
=======
forward_list< automaton_state > automatonEFSM::getNextAutomatonState (const forward_list< automaton_state >& oldStates, const std::map< transition, Events >& event)
>>>>>>> b4d071c5a7f302d3b8bbfb3c70a58825cc28b495
{
forward_list<automaton_state> result;
	//result.push_front(oldStates.front());
	std::map<transition,automaton_state>::iterator next_state;
for (auto oldstate=oldStates.begin();oldstate!=oldStates.end();++oldstate)
{
<<<<<<< HEAD
	for (map<transition,bool>::const_iterator it=event.begin(); it!=event.end(); ++it)
	{ 
		if (it->second==-1 || it->second==1) { //Se l'evento e' undefined o vero, NOTA: l'evento deve per forza essere qualcosa!!
			if ( (next_state=table.internalTable.at(*oldstate).find(it->first)) != table.internalTable.at(*oldstate).end() ){
				result.push_front(next_state->second);
				break;
=======
	for (map<transition,Events>::const_iterator it=event.begin(); it!=event.end(); ++it)
	{ 
		if (it->second==Events::_UNDEFINED || it->second==Events::_TRUE) { //Se l'evento e' undefined o vero, NOTA: l'evento deve per forza essere qualcosa!!
			if ( (next_state=table.internalTable.at(*oldstate).find(it->first)) != table.internalTable.at(*oldstate).end() ){
				result.push_front(next_state->second);
				//break; //Commentare questa riga significa rendere l'automaton non deterministico, cosa che qui vogliamo
>>>>>>> b4d071c5a7f302d3b8bbfb3c70a58825cc28b495
			}
		}
	}
}
result.sort();
result.unique();
return result;
}
