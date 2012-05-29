#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <map>
#include <string>
#include <list>

//if we are debugging we want a strong typedef, if we are releasing we want the code to be optimized
#ifndef NDEBUG
#include <boost/strong_typedef.hpp>
#else
#define BOOST_STRONG_TYPEDEF(a,b) 			\
							typedef a b;
#endif



//Used in the strong_typedef
typedef std::map<int,double> map_int_double;

/**
 * Represents a state of an agent, where the key of the map is the index of the variable as indicated from an indexMap
 */
BOOST_STRONG_TYPEDEF( map_int_double ,agent_state) 
/**
 * Represents a control command, where the key of the map is the index of the variable as indicated from an indexMap
 */
BOOST_STRONG_TYPEDEF( map_int_double ,control_command)


/** This map will be used to store informations about variables names and converting them to int
 * In this way we speed up the read access during simulations
 */
typedef std::map<std::string,int> index_map;

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

struct agent_state_packet
{
	agent_state state;
	std::string identifier;
};

struct variable
{
	std::string name;
	double value;
};

#endif //TYPEDEFS_H