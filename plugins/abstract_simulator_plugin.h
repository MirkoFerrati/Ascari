#ifndef ABSTRACT_SIMULATOR_PLUGIN_H
#define ABSTRACT_SIMULATOR_PLUGIN_H
#include <yaml_parser.h>
#include "../shared/objects/abstract_object.h"


class abstract_simulator_plugin
{
public:
  virtual void run_plugin()=0;
  virtual bool initialize(Parsed_World const& w)=0;
  virtual void stop()=0;
  virtual std::list<abstract_object*>* create_objects()=0;
  virtual std::string get_objects_type()=0;
  virtual ~abstract_simulator_plugin(){};
};

#endif // ABSTRACT_SIMULATOR_PLUGIN_H
