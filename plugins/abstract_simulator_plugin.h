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
  virtual std::list<abstract_object*>* create_objects(){std::list<abstract_object*>* temp=0;return temp;};
  virtual std::string get_objects_type(){return "Error in creating objects";};
  virtual ~abstract_simulator_plugin(){};
};

#endif // ABSTRACT_SIMULATOR_PLUGIN_H
