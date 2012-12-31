#ifndef DECODER_H
#define DECODER_H

#include <map>
#include <string>
#include <typedefs.h>

class decoder
{
public:
	decoder(std::map< int, sub_event_value >* sub_events, std::map< transition, bool >* events);
	decoder();
	void setEventsAndSubEvents(std::map< int, sub_event_value >* sub_events, std::map< transition, bool >* events)
	{
		this->events=events;
		this->sub_events=sub_events;
	};
	void create(std::map<std::string,std::string> events,
		index_map const& sub_events_map,
		std::map<std::string,transition> const& events_map);
	void decode();
	
private:
	std::map<int,sub_event_value>* sub_events;
	std::map<transition,bool>* events;
	
	/**
	 * Dall'indice dell'evento trovo l'indice dei sottoeventi e i loro valori richiesti
	 */
	std::map<int,std::map<int,sub_event_value> > internal_table; //if internal_table[i][j]==sub_events[j] ...
	
};

#endif // DECODER_H