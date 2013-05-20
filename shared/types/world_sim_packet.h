#ifndef WORLD_SIM_PACKET_H
#define WORLD_SIM_PACKET_H
#include "agents_name_to_states.h"
#include <types.h>
#include <objects/task_assignment_task.h>
#include <objects/objects_container.hpp>
#include <map>
#include <string>
struct world_sim_packet {
  
    std::map<std::string,double> bonus_variables;
    agents_name_to_states state_agents;
    simulation_time time;
//    std::map<std::string, task_assignment_task> objects;
  	objects_container object_list;
  
    template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
		ar& time;
        ar& bonus_variables;
        ar& state_agents;
	ar& object_list;
    }
    
};

#endif //WORLD_SIM_PACKET_H