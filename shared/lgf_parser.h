#ifndef LGF_PARSER_H
#define LGF_PARSER_H

#include <lemon/smart_graph.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/dijkstra.h>
#include <lemon/concepts/maps.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include<lemon/graph_to_eps.h>
#include <lemon/color.cc>
#include <logog.hpp>

class Parsed_Graph
{
public:
	lemon::SmartDigraph::NodeMap<int>* coord_x;
    lemon::SmartDigraph::NodeMap<int>* coord_y;
    lemon::SmartDigraph::ArcMap<int>* length;
    lemon::SmartDigraph graph;

};



class lgf_parser
{
};

#endif // LGF_PARSER_H
