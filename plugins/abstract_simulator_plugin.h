#ifndef ABSTRACT_SIMULATOR_PLUGIN_H
#define ABSTRACT_SIMULATOR_PLUGIN_H
#include <yaml_parser.h>
#include "../shared/objects/abstract_object.h"

class simulator;

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


class abstract_simulator_plugin_container
{
public:
    virtual std::string getType()=0;
    virtual abstract_simulator_plugin* getSimulatorPlugin()=0;
    virtual bool createSimulatorPlugin ( simulator* ,Parsed_World* )=0;
    virtual bool createParserPlugin()=0;
    virtual abstract_parser_plugin* getParserPlugin()=0;
    virtual bool isEnabled()=0;
};

#endif // ABSTRACT_SIMULATOR_PLUGIN_H
