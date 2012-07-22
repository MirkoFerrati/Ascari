#include "decoder.h"
#include "logog.hpp"
#include <sstream>

using namespace std;


decoder::decoder(std::map< int, sub_event_value >& sub_events, std::map< transition, bool >& events):sub_events(sub_events),events(events)
{
//Nothing to do here?
}


void decoder::create(map< string, string > events, const index_map& sub_events_map, const std::map< string, transition >& events_map)
{
    map<int,sub_event_value> temp_map;
    for (map<string,transition>::const_iterator it=events_map.begin();it!=events_map.end();it++)
    {
        temp_map.clear();

        stringstream ss(events[it->first]);
        string token;
        while (ss>>token)
        {
            bool negateVariable=false;
            sub_event_value value=_UNDEFINED;
            if (token[0]=='!')
            {
                negateVariable=true;
                token=token.substr(1,token.length()-1);
            }
            if (sub_events_map.count(token))
            {
                if (negateVariable)
                    value=_FALSE;
                else
                    value=_TRUE;
            }
            else
                ERR("evento non esistente %s",token.c_str());
            temp_map.insert(make_pair(sub_events_map.at(token),value));
        }
        internal_table.insert(make_pair(it->second,temp_map));
    }
}

void decoder::decode()
{
    for (map<transition,bool>::iterator it=events.begin(); it!=events.end(); it++)
    {
		it->second=true;
        for (map<int,sub_event_value>::const_iterator iit=internal_table.at(it->first).begin(); iit!=internal_table.at(it->first).end(); iit++)
        {
			if (sub_events.at(iit->first)==_UNDEFINED || iit->second==_UNDEFINED)
				continue;
            if (iit->second!=sub_events.at(iit->first))
            {
				it->second=false;
            }
        }
	}

}
