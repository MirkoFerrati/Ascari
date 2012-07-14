#ifndef AUTOMATONABSTRACT_H
#define AUTOMATONABSTRACT_H
#include <typedefs.h>
#include <vector>
#include <string>
#include <map>

using namespace std;

struct transitionTable
{
	string name;
	map<automaton_state, map<transition, automaton_state> >  internalTable; //newstate=internalTable[oldstate][transition]
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
	virtual vector<automaton_state> getNextAutomatonState(vector<automaton_state>const& oldStates, map<transition,bool>const& event)=0; 
	virtual ~automatonAbstract(){};
protected:
	transitionTable table;
};

#endif // AUTOMATONABSTRACT_H
