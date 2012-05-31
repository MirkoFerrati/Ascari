#ifndef AUTOMATONABSTRACT_H
#define AUTOMATONABSTRACT_H
#include <typedefs.h>
#include <vector>
#include <string>
#include <map>



struct transitionTable
{
	std::map< automaton_state, std::map<transition, automaton_state> >  internalTable; //newstate=internalTable[oldstate][transition]

};

class automatonAbstract
{
public:
	/*!
	 * Creates the automaton with the transitionTable table as internal transition rule
	 * The transitionTable can't be changed, and should not be read
	 */
	automatonAbstract(const transitionTable& table);
	virtual std::vector<automaton_state> getNextAutomatonState(std::vector<automaton_state>const& oldStates,std::vector<transition>const& transitions)=0; 
		
};

#endif // AUTOMATONABSTRACT_H
