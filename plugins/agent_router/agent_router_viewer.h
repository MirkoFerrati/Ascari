#ifndef AGENT_ROUTER_VIEWER_H
#define AGENT_ROUTER_VIEWER_H
#include "viewer.h"
#include <lemon/smart_graph.h>
#include <map>
#include "../abstract_viewer_plugin.h"

class agent_router_viewer:public abstract_viewer_plugin
{
public:
    agent_router_viewer (std::string graphname );

    void init(std::string);
    void paintBackground ( QPainter& painter );
    void setPainterScale ( double scale );
    ~agent_router_viewer();
private:
    void parseGraph (std::string);
    lemon::SmartDigraph graph;
    std::string graphname;

    lemon::SmartDigraph::ArcMap<int> *length;
    lemon::SmartDigraph::NodeMap<int> *coord_x, *coord_y;
};

#endif // AGENT_ROUTER_VIEWER_H
