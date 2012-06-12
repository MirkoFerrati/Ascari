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
			std::map< int, bool > events;
			
			sub_events[1]=_TRUE;
			sub_events[2]=_TRUE;
			sub_events[3]=_TRUE;
			sub_events[4]=_TRUE;
			sub_events[5]=_TRUE;
			
			events[1]=false;
			events[5]=false;
			
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
			assert(events[1]==true);
			assert(events[5]==false);
		}
};

#endif // TESTDECODER_HPP
