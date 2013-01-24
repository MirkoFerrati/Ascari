#include "decoder.h"
#include "logog.hpp"
#include <sstream>

using namespace std;


decoder::decoder(std::map< int, sub_events::value >* sub_events, std::map< transition, events::value >* events):sub_events(sub_events),events(events)
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
    map<int,sub_events::value> temp_map;
    for (map<string,transition>::const_iterator it=events_map.begin();it!=events_map.end();++it)
    {
        temp_map.clear();

        stringstream ss(events_string[it->first]);
        string token;
        while (ss>>token)
        {
            bool negateVariable=false;
            sub_events::value value=sub_events::_UNDEFINED;
            if (token[0]=='!')
            {
                negateVariable=true;
                token=token.substr(1,token.length()-1);
            }
            if (sub_events_map.count(token))
            {
                if (negateVariable)
                    value=sub_events::_FALSE;
                else
                    value=sub_events::_TRUE;
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
    for (map<transition,events::value>::iterator it=events->begin(); it!=events->end(); ++it)
    {
		it->second= events::_TRUE;
        for (map<int,sub_events::value>::const_iterator iit=internal_table.at(it->first).begin(); iit!=internal_table.at(it->first).end(); ++iit)
        {
			if (sub_events->at(iit->first)==sub_events::_UNDEFINED || iit->second==sub_events::_UNDEFINED)
				continue;
            if (iit->second!=sub_events->at(iit->first))
            {
				it->second=events::_FALSE;
            }
        }
	}

}
