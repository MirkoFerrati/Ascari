#ifndef AGENT_ROUTER_VIEWER_H
#define AGENT_ROUTER_VIEWER_H

#include "../HACK_KDEVELOP.h"
#include "../abstract_viewer_plugin.h"

#ifdef ISVIEWER
#include <lemon/smart_graph.h>
#include <map>
#include <yaml_parser.h>



class agent_router_viewer:public abstract_viewer_plugin
{
public:
    agent_router_viewer ();
	agent_router_viewer (Parsed_World* world);
    void init(  );
    void paintBackground ( QPainter& painter );
    void setPainterScale ( double scale );
    ~agent_router_viewer();
private:
    void parseGraph (std::string);
    lemon::SmartDigraph graph;

    lemon::SmartDigraph::ArcMap<int> *length;
    lemon::SmartDigraph::NodeMap<int> *coord_x, *coord_y;
    std::string graphName;
};

#endif
#endif // AGENT_ROUTER_VIEWER_H
