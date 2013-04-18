#ifndef AGENT_SIM_PACKET_H
#define AGENT_SIM_PACKET_H
#include "agent_name_to_ref_states.h"
#include <objects/task_assignment_task.h>

struct agent_sim_packet{
  //pacchetto personalizzato per ogni agente in base alla visibilita'
  const std::map<std::string,double>& bonus_variables;
   agents_name_to_ref_states state_agents;
  const simulation_time& time;
  const std::map<std::string, task_assignment_task>& objects;
    
    agent_sim_packet(const std:: map<std::string,double>& bonus, const simulation_time& t, const std::map<std::string, task_assignment_task>& objects)
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
   std::map<std::string, task_assignment_task> objects;
    
    
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