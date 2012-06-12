#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <map>
#include <string>
#include <list>
#include "boost/serialization/map.hpp" 

//if we are debugging we want a strong typedef, if we are releasing we want the code to be optimized
//we are going to remove all strong_typedef during the code writing
#ifndef NDEBUG
#include <boost/strong_typedef.hpp>
#else
#define BOOST_STRONG_TYPEDEF(a,b) 			\
							typedef a b;
#endif

#define MULTICAST_PORT  30000
#define SINGLECAST_PORT 20000
#define MULTICAST_ADDRESS "239.255.0.1"
#define SIMULATOR_PORT 10000
#define NUM_AGENTS 3
#define T_CAMP 0.01							

//Used in the strong_typedef
typedef std::map<int,double> map_int_double;


/**
 * Represents a state of an agent, where the key of the map is the index of the variable as indicated from an indexMap
 */
struct agent_state: map_int_double
{
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& *this;
	}
};


/**
 * Represents a control command, where the key of the map is the index of the variable as indicated from an indexMap
 */
struct control_command :map_int_double
{
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& *this;
// 		for (std::map<int,double>::const_iterator it= control_map.begin();it!=control_map.end();it++)
// 		{
// 			ar& it->first;
// 			ar& it->second;
// 		}
			
	}
};


/** This map will be used to store informations about variables names and converting them to int
 * In this way we speed up the read access during simulations
 */
typedef std::map<std::string,int> index_map;

enum sub_event_value
{
	_TRUE,
	_FALSE,
	_UNDEFINED
};
	


/**
 * automaton_state is the representation of a discrete state, an indexMap translates the value of automatonState into a string
 */
BOOST_STRONG_TYPEDEF (int, automaton_state)

/**
 * An event is represented by an integer (the name of the event is stored in an IndexMap) and an integer value (extended boolean)
 */
typedef std::map<int,int> events_map;

/**
 * A transition is an event used by the automaton to change his state (automatonState)
 * The map between names of transitions and int is an indexMap
 */
BOOST_STRONG_TYPEDEF( int, transition)


/** The time used by the simulator and sent to agents for synchronization */
typedef double simulation_time;


struct agent_state_packet
{
	agent_state state;
	std::string identifier;
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& state;
		ar& identifier;
	}
};

struct agents_name_to_states
{
	std::map<std::string,agent_state_packet> internal_map;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		for (std::map<std::string,agent_state_packet>::iterator it=internal_map.begin();it!=internal_map.end();it++)
		{
			ar& it->second;	
		}
		
	}
};

struct control_command_packet
{
	control_command command;
	std::string identifier;
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& command;
		ar& identifier;
	}
};

struct variable
{
	std::string name;
	double value;
};

#endif //TYPEDEFS_H