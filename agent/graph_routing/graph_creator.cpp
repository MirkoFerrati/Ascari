#include "graph_creator.h"

#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/dijkstra.h>
#include <lemon/concepts/maps.h>
#include <sstream>
#include <iomanip>
#include<lemon/graph_to_eps.h>
#include <lemon/color.cc>
#include "debug_constants.h"
#include "logog.hpp"

using namespace std;

int Graph_creator::createGraph(int floors)
{
	this->floors=floors;
	try
	{
		parseGraph();
		return graph_node_size;
	}
	catch (lemon::Exception e)
	{
		ERR("Errore nella creazione del grafo: %s",e.what());
		return 0;
	}
}

Graph_creator::Graph_creator(lemon::SmartDigraph &_3Dgraph,lemon::SmartDigraph::ArcMap<int> &length,
    lemon::SmartDigraph::NodeMap<int> &coord_x,lemon::SmartDigraph::NodeMap<int> &coord_y):
    _3Dgraph(_3Dgraph),_3Dlength(length),_3Dcoord_x(coord_x),_3Dcoord_y(coord_y),length(graph),coord_x(graph),
    coord_y(graph)
{
	
}

void Graph_creator::addNodes(lemon::SmartDigraph::NodeMap<lemon::dim2::Point<int> >& coords,
							 lemon::SmartDigraph::NodeMap<int>& ncolors,lemon::SmartDigraph::ArcMap<int>& acolors,int floorNumber)
{
	using namespace lemon;
	for (unsigned int i=0;i<graph_node_size;i++)
	{
		SmartDigraph::Node n = _3Dgraph.addNode();
		//Controllo che tutti i nodi finiscano nella posizione giusta
		assert(_3Dgraph.id(n)==(i+floorNumber*graph_node_size));
		(_3Dcoord_x)[n]=(coord_x)[graph.nodeFromId(i)];
		(_3Dcoord_y)[n]=(coord_y)[graph.nodeFromId(i)];
		coords[n]=dim2::Point<int>((_3Dcoord_x)[n]+(_3Dcoord_y)[n]/3,(_3Dcoord_y)[n]/3+XYOFFSET*(floorNumber+1));
		ncolors[n]=floorNumber+1;
		SmartDigraph::Arc a=_3Dgraph.addArc(n,_3Dgraph.nodeFromId(i));
		(_3Dlength)[a]=10000;
		acolors[a]=floors+3;
	}
}

void Graph_creator::addFloor(lemon::SmartDigraph::NodeMap<lemon::dim2::Point<int> >& coords,
	lemon::SmartDigraph::NodeMap<int>& ncolors,
	lemon::SmartDigraph::ArcMap<int>& acolors,int startId)
{
	using namespace lemon;
	int floorNumber=startId/graph_node_size;
    //Aggiungo un livello al grafo
    addNodes(coords,ncolors,acolors,floorNumber);
	if (floorNumber==0) return;
	//TODO: attenzione, l'ipotesi è che ogni nodo stia sopra il suo gemello con id aumentato del numero di nodi del grafo iniziale
	for (unsigned int i=0;i<graph_node_size;i++)
	{
		vector<int> temp_ids;
		for (SmartDigraph::OutArcIt arcit(graph,graph.nodeFromId(i));arcit!=INVALID;++arcit)
		{
			//archi al piano inferiore
			SmartDigraph::Arc a=_3Dgraph.addArc(_3Dgraph.nodeFromId(i+(floorNumber-1)*graph_node_size),_3Dgraph.nodeFromId(graph.id(graph.target(arcit))+floorNumber*graph_node_size));
			(_3Dlength)[a]=1;
			acolors[a]=floorNumber+1;
			//archi a 2 piani inferiori
			if (floorNumber>1)
			{
				a=_3Dgraph.addArc(_3Dgraph.nodeFromId(i+(floorNumber-2)*graph_node_size),_3Dgraph.nodeFromId(graph.id(graph.target(arcit))+floorNumber*graph_node_size));
				(_3Dlength)[a]=2;
				acolors[a]=floorNumber+1;
			}
		}
		
	}
	
}

void Graph_creator::finalizeFloor(lemon::SmartDigraph::NodeMap<lemon::dim2::Point<int> >& coords,
	lemon::SmartDigraph::NodeMap<int>& ncolors,
	lemon::SmartDigraph::ArcMap<int>& acolors,int startId)
{
	using namespace lemon;
	int floorNumber=startId/graph_node_size;
    //Aggiungo un livello al grafo
    addNodes(coords,ncolors,acolors,floorNumber);
	//TODO: attenzione, l'ipotesi è che ogni nodo stia sopra il suo gemello con id aumentato del numero di nodi del grafo iniziale
	for (unsigned int i=0;i<graph_node_size;i++)
	{
		vector<int> temp_ids;
		for (SmartDigraph::OutArcIt arcit(graph,graph.nodeFromId(i));arcit!=INVALID;++arcit)
		{
			SmartDigraph::Arc a=_3Dgraph.addArc(_3Dgraph.nodeFromId(i+floorNumber*graph_node_size),_3Dgraph.nodeFromId(graph.id(graph.target(arcit))+floorNumber*graph_node_size));
			(_3Dlength)[a]=10; //TODO: ogni numero è fondamentale
			acolors[a]=floorNumber+1;
			a=_3Dgraph.addArc(_3Dgraph.nodeFromId(i+(floorNumber-1)*graph_node_size),_3Dgraph.nodeFromId(graph.id(graph.target(arcit))+floorNumber*graph_node_size));
			(_3Dlength)[a]=1;
			acolors[a]=floorNumber+1;
		}
	}
}

void Graph_creator::parseGraph()
{
	using namespace lemon;

    try {
        digraphReader(graph, GRAPHNAME). // read the directed graph into g
        nodeMap("coordinates_x", coord_x).	//read the coordinates of nodes
        nodeMap("coordinates_y", coord_y).	//read the coordinates of nodes
        arcMap("length", length).       // read the 'capacity' arc map into cap
        run();
    } catch (Exception& er) { // check if there was any error
        ERR("parsing exception %s",er.what());
    }
   	graph_node_size=graph.nodeNum();
	SmartDigraph::NodeMap<dim2::Point<int> > coords(_3Dgraph);
	SmartDigraph::NodeMap<int> ncolors(_3Dgraph,1);
	SmartDigraph::ArcMap<int> acolors(_3Dgraph,1);

	for (unsigned int i=0;i<floors;i++)
	{
		addFloor(coords,ncolors,acolors,graph_node_size*i);
	}
	finalizeFloor(coords,ncolors,acolors,graph_node_size*floors);
	IdMap<SmartDigraph,SmartDigraph::Node> id(_3Dgraph);
	Palette p;	
	lemon::graphToEps<lemon::SmartDigraph>(_3Dgraph,"image.eps").
		coords(coords).
		nodeColors(composeMap(p,ncolors)).
		arcColors(composeMap(p,acolors)).
		nodeTexts(id).
		nodeTextSize(4).
		nodeScale(0.008).
		arcWidthScale(0.0008).
		drawArrows(true).
		arrowWidth(3).
		arrowLength(5).
		enableParallel(true).
		distantColorNodeTexts().
		run();

	std::cout << "A digraph is read from "<<GRAPHNAME << std::endl;
    std::cout << "Number of nodes: " << lemon::countNodes(_3Dgraph) << std::endl;
    std::cout << "Number of arcs: " << lemon::countArcs(_3Dgraph) << std::endl;
}
