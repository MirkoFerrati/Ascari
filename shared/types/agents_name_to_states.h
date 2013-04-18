#ifndef AGENTS_NAME_TO_STATES_H
#define AGENTS_NAME_TO_STATES_H
#include <boost/serialization/access.hpp>
#include "agent_state_packet.h"



struct agents_name_to_states
//TODO(MIRKO): Controllare se questa struttura serve oppure può essere inclusa in World_sim_packet
{
  friend class boost::serialization::access;
  std::map<std::string,agent_state_packet> internal_map;

private:
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar& internal_map;

    }
};
#endif // AGENTS_NAME_TO_STATES_H
