#ifndef PLUGIN_MODULE_H
#define PLUGIN_MODULE_H

#include "exprtk.hpp"
#include <vector>

class Plugin_module
{
public:
	virtual void run_plugin()=0;
	virtual ~Plugin_module(){};
    virtual void addReservedVariables(exprtk::symbol_table< double >& arg1)=0;
	virtual void compileExpressions(exprtk::symbol_table< double >& arg1)=0;
};

#endif // PLUGIN_MODULE_H
