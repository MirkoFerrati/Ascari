#ifndef MONITOR_PACKET_H
#define MONITOR_PACKET_H
#include "monitor_result.h"

#include <deque>
#include <string>

struct monitor_packet
{
	std::string id;
	std::deque<monitor_result> agents;	
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& agents;
		ar& id;
	}
	
};

#endif // MONITOR_PACKET_H
