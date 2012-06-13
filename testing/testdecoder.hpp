#ifndef TESTDECODER_HPP
#define TESTDECODER_HPP
#include "testClass.h"
#include <map>
#include <string>
#include <typedefs.h>
#include "../agent/decoder/decoder.h"

using namespace std;

class testDecoder: public testClass
{
	public:
		void test()
		{
			std::map< int, sub_event_value > sub_events;
			std::map< transition, bool > events;
			
			transition t1=(transition)1;
			transition t2=(transition)2;
			transition t3=(transition)3;
			transition t4=(transition)4;
			transition t5=(transition)5;
			
			
			sub_events[1]=_TRUE;
			sub_events[2]=_TRUE;
			sub_events[3]=_TRUE;
			sub_events[4]=_TRUE;
			sub_events[5]=_TRUE;
			
<<<<<<< .mine
			transition e1 = (transition) 0;
			transition e2 = (transition) 1;
			transition e3 = (transition) 2;
			transition e4 = (transition) 3;
			transition e5 = (transition) 4;
					
			events[e1]=false;
			events[e5]=false;
=======
			events[t1]=false;
			events[t5]=false;
>>>>>>> .r60
			
			std::map<std::string,std::string> parsed_events;
			index_map sub_events_map;
			index_map events_map;

			sub_events_map.insert(make_pair("t1",1));
			sub_events_map.insert(make_pair("t2",5));
			sub_events_map.insert(make_pair("l1",2));
			sub_events_map.insert(make_pair("l2",4));
			sub_events_map.insert(make_pair("l3",3));
			
			events_map.insert(make_pair("e1",1));
			events_map.insert(make_pair("e2",5));
			
			parsed_events.insert(make_pair("e1","t1 l2 l3"));
			parsed_events.insert(make_pair("e2","t2 l1 !l2"));
			
			decoder d(sub_events, events);
			d.create(parsed_events,sub_events_map,events_map);
			
			sub_events[1]=_TRUE;
			sub_events[2]=_UNDEFINED;
			sub_events[3]=_TRUE;
			sub_events[4]=_TRUE;
			sub_events[5]=_FALSE;
			
			d.decode();
<<<<<<< .mine
			assert(events[e1]==true);
			assert(events[e5]==false);
=======
			assert(events[t1]==true);
			assert(events[t5]==false);
>>>>>>> .r60
		}
};

#endif // TESTDECODER_HPP