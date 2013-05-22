#ifndef ABSTRACT_PLUGIN_H
#define ABSTRACT_PLUGIN_H

#include "HACK_KDEVELOP.h"

#ifdef ISAGENT
#include "abstract_agent_plugin.h"
#include "../agent/agent.h"
#endif

#ifdef ISSIMULATOR
#include "abstract_simulator_plugin.h"
#include "../simulator/simulator.h"
#endif

#ifdef ISVIEWER
#include "abstract_viewer_plugin.h"
#include "../viewer/gui/viewer.h"
#endif

#include "abstract_parser_plugin.h"



class abstract_plugin
{
public:
#ifdef ISAGENT
    virtual abstract_agent_plugin* getAgentPlugin() =0;
    virtual bool createAgentPlugin ( agent* a,Parsed_World* parse ){return false;};
#endif

#ifdef ISSIMULATOR
    virtual abstract_simulator_plugin* getSimulatorPlugin() =0;
    virtual bool createSimulatorPlugin ( simulator* a,Parsed_World* parse ){return false;};
#endif

#ifdef ISVIEWER
    virtual abstract_viewer_plugin* getViewerPlugin() =0;
    virtual bool createViewerPlugin ( Viewer* a, Parsed_World* parse=0 ){return false;};
#endif

    virtual abstract_parser_plugin* getParserPlugin() =0;
    virtual bool createParserPlugin()=0;
    virtual std::string getType()=0;
    virtual ~abstract_plugin(){};
};

#endif // ABSTRACT_PLUGIN_H
