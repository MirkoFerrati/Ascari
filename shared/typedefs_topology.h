#ifndef TYPEDEF_TOPOLOGY_H
#define TYPEDEF_TOPOLOGY_H

#include <boost/asio.hpp>
#include <boost/serialization/map.hpp>
#include <string>

typedef std::string agent_name;
typedef std::string behavior_name;
typedef std::string topology_name;
typedef std::map<topology_name, short > topology_values;
typedef std::map<behavior_name, topology_values> behavior_topology;

struct topology_packet
{
	std::map<agent_name, behavior_topology> data;
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& data;
	}
};


#endif //TYPEDEF_TOPOLOGY_H