#ifndef ABSTRACT_AGENT_PLUGIN_H
#define ABSTRACT_AGENT_PLUGIN_H


#include "HACK_KDEVELOP.h"
#ifdef ISAGENT

#include "exprtk.hpp"
#include <vector>


class abstract_agent_plugin
{
  public:
	virtual void run_plugin()=0;
	virtual ~abstract_agent_plugin(){};
	virtual bool initialize(){return true;};
};

#endif
#endif // ABSTRACT_AGENT_PLUGIN_H
