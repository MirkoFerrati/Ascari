#include "graph_creator.h"

#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/dijkstra.h>
#include <lemon/concepts/maps.h>
#include <sstream>
#include <iomanip>
#include <boost/algorithm/string.hpp>

#include<lemon/graph_to_eps.h>
#include <lemon/color.cc>
#include "debug_constants.h"
#include "logog.hpp"

#define sqr(a) (a)*(a)
#define MAX_ARC_FLOORS 2

using namespace std;

int Graph_creator::createGraph ( int floors, string graphName )
{
    this->floors=floors;
    try
    {
        parseGraph ( graphName );
        return graph_node_size;
    }
    catch ( lemon::Exception const& e )
    {
        ERR ( "Errore nella creazione del grafo: %s",e.what() );
        return 0;
    }
}

Graph_creator::Graph_creator ( lemon::SmartDigraph &_3Dgraph,lemon::SmartDigraph::ArcMap<int> &length,
                               lemon::SmartDigraph::NodeMap<int> &coord_x,lemon::SmartDigraph::NodeMap<int> &coord_y ) :
    _3Dgraph ( _3Dgraph ),_3Dlength ( length ),_3Dcoord_x ( coord_x ),_3Dcoord_y ( coord_y ),length ( graph ),coord_x ( graph ),
    coord_y ( graph )
{
    floors=0;
    graph_node_size=0;
}

void Graph_creator::addNodes ( lemon::SmartDigraph::NodeMap<lemon::dim2::Point<int> >& coords,
                               lemon::SmartDigraph::NodeMap<int>& ncolors,lemon::SmartDigraph::ArcMap<int>& acolors,int floorNumber )
{
    using namespace lemon;
    for ( unsigned int i=0; i<graph_node_size; i++ )
    {
        SmartDigraph::Node n = _3Dgraph.addNode();
        //Controllo che tutti i nodi finiscano nella posizione giusta
        assert ( _3Dgraph.id ( n ) == ( i+floorNumber*graph_node_size ) );
        ( _3Dcoord_x ) [n]= ( coord_x ) [graph.nodeFromId ( i )];
        ( _3Dcoord_y ) [n]= ( coord_y ) [graph.nodeFromId ( i )];
        coords[n]=dim2::Point<int> ( ( _3Dcoord_x ) [n]+ ( _3Dcoord_y ) [n]/3, ( _3Dcoord_y ) [n]/3+XYOFFSET* ( floorNumber+1 ) );
        ncolors[n]=floorNumber+1;
        if ( floorNumber>0 )
        {
            SmartDigraph::Arc a=_3Dgraph.addArc ( n,_3Dgraph.nodeFromId ( i ) );
            ( _3Dlength ) [a]=VERTICAL_LENGTH;
            acolors[a]=floors+3;
        }
    }
}

void Graph_creator::addFloor ( lemon::SmartDigraph::NodeMap<lemon::dim2::Point<int> >& coords,
                               lemon::SmartDigraph::NodeMap<int>& ncolors,
                               lemon::SmartDigraph::ArcMap<int>& acolors,int startId )
{
    using namespace lemon;
    unsigned int floorNumber=startId/graph_node_size;
    //Aggiungo un livello al grafo
    addNodes ( coords,ncolors,acolors,floorNumber );
    if ( floorNumber==0 ) return;
    //TODO: attenzione, l'ipotesi e' che ogni nodo stia sopra il suo gemello con id aumentato del numero di nodi del grafo iniziale
    SmartDigraph::Node source;
    SmartDigraph::Node target;
    for ( unsigned int i=0; i<graph_node_size; i++ )
    {
        for ( SmartDigraph::OutArcIt arcit ( graph,graph.nodeFromId ( i ) ); arcit!=INVALID; ++arcit )
        {
            for ( unsigned int j=0; j<MAX_ARC_FLOORS; j++ )
            {
                target=_3Dgraph.nodeFromId ( graph.id ( graph.target ( arcit ) ) +floorNumber*graph_node_size );
                //archi al piano inferiore
                if ( floorNumber>j )
                {
                    source=_3Dgraph.nodeFromId ( i+ ( floorNumber-1-j ) *graph_node_size );
                    SmartDigraph::Arc a=_3Dgraph.addArc ( source,target );
                    ( _3Dlength ) [a]= ( 1+j*1.1 ) *sqrt ( ( double ) sqr ( _3Dcoord_x[source]-_3Dcoord_x[target] ) + ( double ) sqr ( _3Dcoord_y[source]-_3Dcoord_y[target] ) ); //1; prendo la distanza reale invece dell'unità
                    acolors[a]=floorNumber+1;
                }
                //archi a 2 piani inferiori

// 			if (floorNumber>1)
// 			{
// 				a=_3Dgraph.addArc(_3Dgraph.nodeFromId(i+(floorNumber-2)*graph_node_size),target);
// 				(_3Dlength)[a]=2.1*sqrt(sqr(_3Dcoord_x[source]-_3Dcoord_x[target])+sqr(_3Dcoord_y[source]-_3Dcoord_y[target]));//2;
// 				acolors[a]=floorNumber+1;
// 			}
// 			//archi a 3 piani
//
// 			if (floorNumber>2)
// 			{
// 				a=_3Dgraph.addArc(_3Dgraph.nodeFromId(i+(floorNumber-3)*graph_node_size),target);
// 				(_3Dlength)[a]=3.3*sqrt(sqr(_3Dcoord_x[source]-_3Dcoord_x[target])+sqr(_3Dcoord_y[source]-_3Dcoord_y[target]));//2;
// 				acolors[a]=floorNumber+1;
// 			}
            }
        }

    }

}

void Graph_creator::finalizeFloor ( lemon::SmartDigraph::NodeMap<lemon::dim2::Point<int> >& coords,
                                    lemon::SmartDigraph::NodeMap<int>& ncolors,
                                    lemon::SmartDigraph::ArcMap<int>& acolors,int startId )
{
    using namespace lemon;
    int floorNumber=startId/graph_node_size;
    //Aggiungo un livello al grafo
    addNodes ( coords,ncolors,acolors,floorNumber );
    //TODO: attenzione, l'ipotesi e' che ogni nodo stia sopra il suo gemello con id aumentato del numero di nodi del grafo iniziale
    SmartDigraph::Node source;
    SmartDigraph::Node target;
    for ( unsigned int i=0; i<graph_node_size; i++ )
    {
        for ( SmartDigraph::OutArcIt arcit ( graph,graph.nodeFromId ( i ) ); arcit!=INVALID; ++arcit )
        {
            SmartDigraph::Arc a=_3Dgraph.addArc ( _3Dgraph.nodeFromId ( i+floorNumber*graph_node_size ),_3Dgraph.nodeFromId ( graph.id ( graph.target ( arcit ) ) +floorNumber*graph_node_size ) );
            ( _3Dlength ) [a]=TOP_FLOOR_LENGTH; //TODO: ogni numero e' fondamentale
            acolors[a]=floorNumber+1;
            source=_3Dgraph.nodeFromId ( i+ ( floorNumber-1 ) *graph_node_size );
            target=_3Dgraph.nodeFromId ( graph.id ( graph.target ( arcit ) ) +floorNumber*graph_node_size );
            a=_3Dgraph.addArc ( source,target );
            ( _3Dlength ) [a]=sqrt ( ( double ) sqr ( _3Dcoord_x[source]-_3Dcoord_x[target] ) + ( double ) sqr ( _3Dcoord_y[source]-_3Dcoord_y[target] ) ); //1; prendo la distanza reale invece dell'unità;
            acolors[a]=floorNumber+1;
            a=_3Dgraph.addArc ( _3Dgraph.nodeFromId ( i+ ( floorNumber-2 ) *graph_node_size ),target );
            ( _3Dlength ) [a]=2.1*sqrt ( sqr ( _3Dcoord_x[source]-_3Dcoord_x[target] ) +sqr ( _3Dcoord_y[source]-_3Dcoord_y[target] ) ); //2;
            acolors[a]=floorNumber+1;
            a=_3Dgraph.addArc ( _3Dgraph.nodeFromId ( i+ ( floorNumber-3 ) *graph_node_size ),target );
            ( _3Dlength ) [a]=3.3*sqrt ( sqr ( _3Dcoord_x[source]-_3Dcoord_x[target] ) +sqr ( _3Dcoord_y[source]-_3Dcoord_y[target] ) ); //2;
            acolors[a]=floorNumber+1;
        }
    }
}

void Graph_creator::parseGraph ( string graphName )
{
    using namespace lemon;
    boost::algorithm::to_lower ( graphName );

    try
    {
        digraphReader ( graph, graphName ). // read the directed graph into g
        nodeMap ( "coordinates_x", coord_x ).	//read the coordinates of nodes
        nodeMap ( "coordinates_y", coord_y ).	//read the coordinates of nodes
        arcMap ( "length", length ).    // read the 'capacity' arc map into cap
        run();
    }
    catch ( Exception& er )   // check if there was any error
    {
        ERR ( "parsing exception %s",er.what() );
    }
    graph_node_size=graph.nodeNum();
    SmartDigraph::NodeMap<dim2::Point<int> > coords ( _3Dgraph );
    SmartDigraph::NodeMap<int> ncolors ( _3Dgraph,1 );
    SmartDigraph::ArcMap<int> acolors ( _3Dgraph,1 );

    for ( unsigned int i=0; i<floors; i++ )
    {
        addFloor ( coords,ncolors,acolors,graph_node_size*i );
    }
    finalizeFloor ( coords,ncolors,acolors,graph_node_size*floors );
    IdMap<SmartDigraph,SmartDigraph::Node> id ( _3Dgraph );
    Palette p;
    lemon::graphToEps<lemon::SmartDigraph> ( _3Dgraph,"image.eps" ).
    coords ( coords ).
    nodeColors ( composeMap ( p,ncolors ) ).
    arcColors ( composeMap ( p,acolors ) ).
    nodeTexts ( id ).
    nodeTextSize ( 4 ).
    nodeScale ( 0.008 ).
    arcWidthScale ( 0.0008 ).
    drawArrows ( true ).
    arrowWidth ( 3 ).
    arrowLength ( 5 ).
    enableParallel ( true ).
    distantColorNodeTexts().
    run();

    std::cout << "A digraph is read from "<<graphName << std::endl;
    std::cout << "Number of nodes: " << lemon::countNodes ( _3Dgraph ) << std::endl;
    std::cout << "Number of arcs: " << lemon::countArcs ( _3Dgraph ) << std::endl;
}
