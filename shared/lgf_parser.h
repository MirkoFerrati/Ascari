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

void parseGraph(string filename,Parsed_Graph& p)
{
    p.coord_x= new lemon::SmartDigraph::NodeMap<int>(p.graph);
    p.coord_y= new lemon::SmartDigraph::NodeMap<int>(p.graph);
    p.length= new lemon::SmartDigraph::ArcMap<int>(p.graph);

    try {
        lemon::digraphReader(p.graph, filename). // read the directed graph into g
        nodeMap("coordinates_x", *p.coord_x).	//read the coordinates of nodes
        nodeMap("coordinates_y", *p.coord_y).	//read the coordinates of nodes
        arcMap("length", *p.length).       // read the 'capacity' arc map into cap
        run();
    } catch (lemon::Exception& er) { // check if there was any error
       ERR("parsing exception %s",er.what());
    }

    std::cout << "A digraph is read from "<<filename << std::endl;
    std::cout << "Number of nodes: " << lemon::countNodes(p.graph) << std::endl;
    std::cout << "Number of arcs: " << lemon::countArcs(p.graph) << std::endl;
}



class lgf_parser
{
};

#endif // LGF_PARSER_H
