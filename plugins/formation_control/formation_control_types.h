#ifndef FORMATION_CONTROL_TYPES_H
#define FORMATION_CONTROL_TYPES_H

#include "streams_utils.h"

#include <map>
#include <string>
#include <vector>
#include <list>
#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/deque.hpp"
#include <exprtk.hpp>
#include "types.h"
#include <types/agent_state_packet.h>
#include <iostream>
#include <fstream>
#include <forward_list>


#define STATE_X 1
#define STATE_Y 2
#define STATE_THETA 3

class formation_control_packet
{
public:
  agent_state_packet agent_state;
    
  template <typename Archive>
  void serialize(Archive& ar, const unsigned int /*version*/)
  {
      ar& agent_state;
  }
};

#endif