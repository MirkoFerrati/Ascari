#ifndef DEFINE_H
#define DEFINE_H
// 
// #include <map>
// #include <string>
// #include <vector>
// #include <list>
// #include "boost/serialization/map.hpp"
// #include "boost/serialization/vector.hpp"
// #include "boost/serialization/deque.hpp"
// #include <exprtk.hpp>
// #include "objects/abstract_object.h"
// #include "objects/task_assignment_task.h"
// #include "objects/task_assignment_task.h"
// #include <iostream>
// #include <fstream>
// #include <forward_list>
// #include "global_types.h"
// #include "task_assignment_types.h"
// #include "types.h"
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
#define SOCKET_BINDING "0.0.0.0"
#define MAX_PACKET_LENGTH 1024*32
#define SIMULATOR_PORT 10000
#define SIMULATOR_ROUTE_PORT 10050
#define AGENT_ROUTE_PORT 10051
#define SIMULATOR_GRAPH_PORT 10052
#define AGENT_GRAPH_PORT 10053
#define NUM_AGENTS 1
#define T_CAMP 0.01
#define SYNC_PROTOCOL "tcp://localhost:5761"
							
//written by Alessandro Settimi
#define AGENT_TA_PORT 5580
#define SIMULATOR_TA_PORT 5581
//written by Alessandro Settimi





#endif //DEFINE_H
