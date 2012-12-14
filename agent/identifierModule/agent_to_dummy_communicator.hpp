#ifndef AGENT_TO_DUMMY_COMMUNICATOR
#define AGENT_TO_DUMMY_COMMUNICATOR

#include "agent.h"
#include "typedefs.h"
class agent_to_dummy_communicator

{
public:
  //Blocking call, for now...
  void send(int dummy_identifier,agents_name_to_states const& infos)
  {
    
  }
    control_command_packet receive (int dummy_identifier)
    {
      
    }
  
  
  
};










#endif //AGENT_TO_DUMMY_COMMUNICATOR