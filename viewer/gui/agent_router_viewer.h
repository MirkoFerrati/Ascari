#ifndef AGENT_ROUTER_VIEWER_H
#define AGENT_ROUTER_VIEWER_H
#include "viewer_plugin.h"
#include "viewer.h"
#include <lemon/smart_graph.h>
#include <map>

class agent_router_viewer:public viewer_plugin
{
public:
    agent_router_viewer (std::string graphname );

    void init(std::string);
    void paintBackground ( QPainter& painter );

    ~agent_router_viewer();
private:
    void parseGraph (std::string);
    lemon::SmartDigraph graph;
    std::string graphname;

    lemon::SmartDigraph::ArcMap<int> *length;
    lemon::SmartDigraph::NodeMap<int> *coord_x, *coord_y;
};

#endif // AGENT_ROUTER_VIEWER_H
