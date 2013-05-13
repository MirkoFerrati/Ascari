#ifndef ABSTRACT_PLUGIN_H
#define ABSTRACT_PLUGIN_H
#include "abstract_agent_plugin.h"
#include "abstract_simulator_plugin.h"
#include "abstract_viewer_plugin.h"
#include "abstract_parser_plugin.h"


class abstract_plugin
{
public:
  virtual abstract_agent_plugin* getAgentPlugin()=0;
  virtual abstract_simulator_plugin* getSimulatorPlugin()=0;
  virtual abstract_viewer_plugin* getViewerPlugin()=0;
  virtual abstract_parser_plugin* getParserPlugin()=0;
  
  
  
};

#endif // ABSTRACT_PLUGIN_H
