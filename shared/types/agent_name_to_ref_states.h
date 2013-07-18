#ifndef AGENT_NAME_TO_REF_STATES_H
#define AGENT_NAME_TO_REF_STATES_H
#include "agent_state_packet.h"
#include <boost/serialization/access.hpp>



struct agents_name_to_ref_states
//TODO(MIRKO): Controllare se questa struttura serve oppure puo' essere inclusa in World_sim_packet
{
  friend class boost::serialization::access;
  std::map<std::string,agent_state_packet*> internal_map;

private:
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar& internal_map;

    }
};


#endif // AGENT_NAME_TO_REF_STATES_H
