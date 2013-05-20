#ifndef ABSTRACT_AGENT_PLUGIN_H
#define ABSTRACT_AGENT_PLUGIN_H


#include "abstract_plugin.h"
#ifdef ISAGENT
#include "exprtk.hpp"
#include <vector>


class abstract_agent_plugin
{
  public:
	virtual void run_plugin()=0;
	virtual ~abstract_agent_plugin(){};
    virtual void addReservedVariables(exprtk::symbol_table< double >& arg1)=0;
	virtual void compileExpressions(exprtk::symbol_table< double >& arg1)=0;

};

#endif
#endif // ABSTRACT_AGENT_PLUGIN_H
