#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <string>
typedef std::map<int,double> map_int_double;
typedef std::map<int,double> agent_state;
typedef std::map<int,double> control_command;
typedef std::string target_abstract;
/** This map will be used to store informations about variables names and converting them to int
 * In this way we speed up the read access during simulations
 */
typedef std::map<std::string, int> index_map;


/** The time used by the simulator and sent to agents for synchronization */
typedef double simulation_time;


#endif