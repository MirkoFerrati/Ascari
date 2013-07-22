#ifndef AGENT_SIM_PACKET_H
#define AGENT_SIM_PACKET_H
#include "agent_name_to_ref_states.h"
#include <objects/task_assignment_task.h>
#include <objects/objects_container.hpp>

struct agent_sim_packet{
  //pacchetto personalizzato per ogni agente in base alla visibilita'
  const std::map<std::string,double>& bonus_variables;
   agents_name_to_ref_states state_agents;
  const simulation_time& time;
    
  objects_container& objects;
  
    agent_sim_packet(const std:: map<std::string,double>& bonus, const simulation_time& t, objects_container& objects)
    :bonus_variables(bonus),time(t),objects(objects)
    {
	
    }
    
    template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
	ar& time;
        ar& bonus_variables;
        ar& state_agents;
	ar& objects;
    }
    
};

struct agent_sim_packet_receiver{
  //pacchetto personalizzato per ogni agente in base alla visibilita'
  std::map<std::string,double> bonus_variables;
   agents_name_to_ref_states state_agents;
  simulation_time time;
   objects_container objects;
    
    
    template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
	ar& time;
        ar& bonus_variables;
        ar& state_agents;
	ar& objects;
    }
    
};


#endif //agent_sim_packet_h