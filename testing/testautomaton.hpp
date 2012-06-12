#ifndef TESTAUTOMATON_HPP
#define TESTAUTOMATON_HPP

#include <../agent/automaton/automatonFSM.h>

#include <vector>
#include <map>
#include <string>
#include "typedefs.h"
#include "testClass.h"

using namespace std;

class testAutomaton:public testClass
{
public:
    void test() {
		
		transitionTable trans_table;
		string name;
		name="RH";
		
		automaton_state fast=(automaton_state) 0;
		automaton_state slow=(automaton_state) 1;
		automaton_state left=(automaton_state) 2;
		
		transition e1=(transition) 0;
		transition e2=(transition) 1;
		transition e3=(transition) 2;
		
		map<transition, automaton_state> tmp;
		trans_table.internalTable[fast].insert(make_pair<transition,automaton_state>(e1,fast));
		
		trans_table.internalTable[fast].insert(make_pair<transition,automaton_state>(e2,slow));
		
		trans_table.internalTable[slow].insert(make_pair<transition,automaton_state>(e1,left));
		
		trans_table.internalTable[slow].insert(make_pair<transition,automaton_state>(e2,slow));
		
		trans_table.internalTable[slow].insert(make_pair<transition,automaton_state>(e3,fast));

		trans_table.internalTable[left].insert(make_pair<transition,automaton_state>(e3,slow));
		
		trans_table.internalTable[left].insert(make_pair<transition,automaton_state>(e1,fast));
		
		
		automatonFSM test_automaton(trans_table,name);
		
		vector<automaton_state> new_state;
		
		
		vector<automaton_state> old_state;
		vector<transition> trans;
		
		old_state.push_back(fast);
		trans.push_back(e1);
		
		new_state=test_automaton.getNextAutomatonState(old_state,trans);
		
		assert(new_state.at(0)==fast);
	
		old_state[0]=slow;
		trans[0]=e2;
		
		new_state=test_automaton.getNextAutomatonState(old_state,trans);
		
		assert(new_state.at(0)==slow);
		
		old_state[0]=left;
		trans[0]=e3;
		
		new_state=test_automaton.getNextAutomatonState(old_state,trans);
		
		assert(new_state.at(0)==slow);
		
		
		}
		
};
	
	#endif // TESTAUTOMATON_HPP