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
// 		string name;
// 		name = "RH";
		
		automaton_state fast = (automaton_state) 0;
		automaton_state slow = (automaton_state) 1;
		automaton_state left = (automaton_state) 2;
		
		transition e1 = (transition) 0;
		transition e2 = (transition) 1;
		transition e3 = (transition) 2;
		
		//map<transition, automaton_state> tmp;
		trans_table.name = "A1";
		trans_table.internalTable[fast].insert(make_pair(e1,fast));
		trans_table.internalTable[fast].insert(make_pair(e2,slow));
		
		trans_table.internalTable[slow].insert(make_pair(e1,left));
		trans_table.internalTable[slow].insert(make_pair(e2,slow));
		trans_table.internalTable[slow].insert(make_pair(e3,fast));

		trans_table.internalTable[left].insert(make_pair(e3,slow));
		trans_table.internalTable[left].insert(make_pair(e1,fast));
		
		map<transition,bool> events_Status;
		events_Status.insert(make_pair(e1,0));
		events_Status.insert(make_pair(e2,0));
		events_Status.insert(make_pair(e3,0));
		
		automatonFSM test_automaton(trans_table);
		forward_list<automaton_state> new_state;
		forward_list<automaton_state> old_state;
		//vector<transition> trans;
		
		old_state.push_front(fast);
		//trans.push_back(e1);
		events_Status[e1] = 1;
		new_state=test_automaton.getNextAutomatonState(old_state,events_Status);
		assert(new_state.front()==fast);
	
		old_state.front()=slow;
		//trans[0]=e2;
		events_Status[e1] = 0;
		events_Status[e2] = 1;
		new_state=test_automaton.getNextAutomatonState(old_state,events_Status);
		assert(new_state.front()==slow);
		
		old_state.front()=left;
		//trans[0]=e3;
		events_Status[e2] = 0;
		events_Status[e3] = 1;
		new_state=test_automaton.getNextAutomatonState(old_state,events_Status);
		assert(new_state.front()==slow);
	}	
};
	
	#endif // TESTAUTOMATON_HPP