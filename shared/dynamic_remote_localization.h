#ifndef DYNAMIC_REMOTE_LOCALIZATION_H
#define DYNAMIC_REMOTE_LOCALIZATION_H
#include "dynamic_module_abstract.h"

class dynamic_remote_localization : public dynamic_module_abstract
{
    dynamic_remote_localization();
    virtual agent_state getNextState();
};

#endif // DYNAMIC_REMOTE_LOCALIZATION_H
