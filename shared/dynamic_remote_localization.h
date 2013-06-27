#ifndef DYNAMIC_REMOTE_LOCALIZATION_H
#define DYNAMIC_REMOTE_LOCALIZATION_H
#include "dynamic_module_abstract.h"

#include <communication/zmq_localization_communicator.hpp>

class dynamic_remote_localization : public dynamic_module_abstract
{
public:
    dynamic_remote_localization(zmq_localization_communicator_receiver* receiver, std::string agent_name,agent_state  state);
    virtual agent_state getNextState();
private:
    
private:
    zmq_localization_communicator_receiver* receiver;
    std::string my_agent;
};

#endif // DYNAMIC_REMOTE_LOCALIZATION_H
