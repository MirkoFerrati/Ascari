#ifndef ABSTRACT_SIMULATOR_PLUGIN_H
#define ABSTRACT_SIMULATOR_PLUGIN_H
#include <yaml_parser.h>

class abstract_simulator_plugin
{
public:
  virtual void run_plugin()=0;
  virtual bool initialize(Parsed_World const& w)=0;
  virtual void stop()=0;
  virtual ~abstract_simulator_plugin(){};
};

#endif // ABSTRACT_SIMULATOR_PLUGIN_H
