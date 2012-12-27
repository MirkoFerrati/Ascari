#ifndef AGENT_TO_DUMMY_COMMUNICATOR
#define AGENT_TO_DUMMY_COMMUNICATOR

#include "agent.h"
#include "dummy_agent.hpp"
#include "typedefs.h"
#include <mutex>
class agent_to_dummy_communicator

{
public:

    agent_to_dummy_communicator (){

    }

    //Blocking call, for now...
    void send (dummy_agent & target, agents_name_to_states const& infos) {

    }
    control_command_packet receive (dummy_agent & source) {
        source.comm_mutex.lock();





        source.comm_mutex.unlock();

    }
private:


};










#endif //AGENT_TO_DUMMY_COMMUNICATOR
