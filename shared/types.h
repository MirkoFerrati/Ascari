#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <string>
typedef std::map<int,double> map_int_double;
typedef std::map<int,double> agent_state;
typedef std::map<int,double> control_command;
typedef std::string discrete_state;
typedef std::string target_abstract;
/** This map will be used to store informations about variables names and converting them to int
 * In this way we speed up the read access during simulations
 */
typedef std::map<std::string, int> index_map;






/**
 * automaton_state is the representation of a discrete state, an indexMap translates the value of automatonState into a string
 */
typedef int automaton_state;



/**
 * A transition is an event used by the automaton to change his state (automatonState)
 * The map between names of transitions and int is an indexMap
 */
typedef int transition;


/** The time used by the simulator and sent to agents for synchronization */
typedef double simulation_time;






#endif