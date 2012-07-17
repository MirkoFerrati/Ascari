#ifndef PLUGIN_MODULE_H
#define PLUGIN_MODULE_H

class Plugin_module
{
public:
	virtual void run_plugin()=0;
	virtual ~Plugin_module(){};
};

#endif // PLUGIN_MODULE_H
