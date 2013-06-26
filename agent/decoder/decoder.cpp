#include "decoder.h"
#include "logog.hpp"
#include <sstream>

using namespace std;


decoder::decoder(std::map< int, Sub_events >* sub_events, map< transition, Events >* events):sub_events(sub_events),events(events)
{
//Nothing to do here?
}

decoder::decoder()
{
	events=0;
	sub_events=0;
}


void decoder::create(map< string, string > events_string, const index_map& sub_events_map, const std::map< string, transition >& events_map)
{
	//Impongo il set di events e subevents prima della create, sperando basti
	if (!events || !sub_events)
	{
		WARN("errore, prima di usare decoder::create si prega di settare events e sub_events",NULL);
	}
    map<int,Sub_events> temp_map;
    for (map<string,transition>::const_iterator it=events_map.begin();it!=events_map.end();++it)
    {
        temp_map.clear();

        stringstream ss(events_string[it->first]);
        string token;
        while (ss>>token)
        {
            bool negateVariable=false;
            Sub_events value=Sub_events::_UNDEFINED;
            if (token[0]=='!')
            {
                negateVariable=true;
                token=token.substr(1,token.length()-1);
            }
            if (sub_events_map.count(token))
            {
                if (negateVariable)
                    value=Sub_events::_FALSE;
                else
                    value=Sub_events::_TRUE;
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
    for (map<transition,Events>::iterator it=events->begin(); it!=events->end(); ++it)
    {
		it->second= Events::_TRUE;
        for (map<int,Sub_events>::const_iterator iit=internal_table.at(it->first).begin(); iit!=internal_table.at(it->first).end(); ++iit)
        {
			if (sub_events->at(iit->first)==Sub_events::_UNDEFINED || iit->second==Sub_events::_UNDEFINED)
				continue;
            if (iit->second!=sub_events->at(iit->first))
            {
				it->second=Events::_FALSE;
            }
        }
	}

}
