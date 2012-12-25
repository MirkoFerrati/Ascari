#ifndef AUTOMATONABSTRACT_H
#define AUTOMATONABSTRACT_H
#include <typedefs.h>
#include <vector>
#include <string>
#include <map>
#include <forward_list>


struct transitionTable
{
	std::string name;
	std::map<automaton_state, std::map<transition, automaton_state> >  internalTable; //newstate=internalTable[oldstate][transition]
};

class automatonAbstract
{
public:
	/*!
	 * Creates the automaton with the transitionTable table as internal transition rule
	 * The transitionTable can't be changed, and should not be read
	 */
	automatonAbstract(const transitionTable& table)
	:table(table){}
	virtual std::forward_list<automaton_state> getNextAutomatonState(std::forward_list<automaton_state>const& oldStates, std::map<transition,bool>const& event)=0; 
	virtual ~automatonAbstract(){};
protected:
	transitionTable table;
};

#endif // AUTOMATONABSTRACT_H
