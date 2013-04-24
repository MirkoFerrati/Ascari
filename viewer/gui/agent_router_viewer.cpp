#include "agent_router_viewer.h"
#include "viewer.h"
#include <lemon/lgf_reader.h>

using namespace std;

agent_router_viewer::agent_router_viewer()
{
  father=0;
     length=0;
    coord_x=0;
    coord_y=0;
}

void agent_router_viewer::init(std::string filename)
{
  assert(father);
  Viewer* temp_father=reinterpret_cast<Viewer*>(father);
    parseGraph (filename);
	lemon::SmartDigraph::NodeIt n ( graph );
	double maxx=( *coord_x ) [n],maxy=( *coord_y ) [n],minx=( *coord_x ) [n],miny=( *coord_y ) [n];
    for ( lemon::SmartDigraph::NodeIt n ( graph ); n!=lemon::INVALID; ++n )
    {
        if ( maxx< ( *coord_x ) [n] ) maxx= ( *coord_x ) [n];
        if ( maxy< ( *coord_y ) [n] ) maxy= ( *coord_y ) [n];
        if ( minx> ( *coord_x ) [n] ) minx= ( *coord_x ) [n];
        if ( miny> ( *coord_y ) [n] ) miny= ( *coord_y ) [n];
    }
    temp_father->setScalingAndTranslateFactor(maxx,minx,maxy,miny);
}

void agent_router_viewer::paintBackground ( QPainter& painter )
{
  assert(father);
        painter.save();
        painter.setBrush ( QColor ( "lightgreen" ) );

        for ( lemon::SmartDigraph::NodeIt n ( graph ); n!=lemon::INVALID; ++n )
        {
            painter.save();
            painter.translate ( ( *coord_x ) [n], ( *coord_y ) [n] );
			
			painter.scale(4,4);
            painter.drawEllipse ( QPoint ( 0,0 ),1,1 );
			painter.scale(1/4.0,1/4.0);
            painter.scale ( painter.fontMetrics().height() /10.0,-painter.fontMetrics().height() /10.0 );
            painter.drawText ( -1,-1,QString ( "" ).setNum ( graph.id ( n ) ) );
            painter.restore();
        }

        painter.setBrush ( QColor ( "black" ) );
        for ( lemon::SmartDigraph::ArcIt a ( graph ); a!=lemon::INVALID; ++a )
        {
            painter.drawLine ( ( *coord_x ) [graph.source ( a )], ( *coord_y ) [graph.source ( a )], ( *coord_x ) [graph.target ( a )], ( *coord_y ) [graph.target ( a )] );
        }

        painter.restore();
}


void agent_router_viewer::parseGraph (std::string graphname)
{
    coord_x= new lemon::SmartDigraph::NodeMap<int> ( graph );
    coord_y= new lemon::SmartDigraph::NodeMap<int> ( graph );
    length= new lemon::SmartDigraph::ArcMap<int> ( graph );

    try
    {
        lemon::digraphReader ( graph, graphname ). // read the directed graph into g
        nodeMap ( "coordinates_x", *coord_x ).	//read the coordinates of nodes
        nodeMap ( "coordinates_y", *coord_y ).	//read the coordinates of nodes
        arcMap ( "length", *length ).    // read the 'capacity' arc map into cap
        run();
    }
    catch ( lemon::Exception& er )   // check if there was any error
    {
    }

    std::cout << "A digraph is read from "<<graphname << std::endl;
    std::cout << "Number of nodes: " << lemon::countNodes ( graph ) << std::endl;
    std::cout << "Number of arcs: " << lemon::countArcs ( graph ) << std::endl;
}


agent_router_viewer::~agent_router_viewer()
{
  
      if ( length )
        delete length;
    if ( coord_x )
        delete coord_x;
    if ( coord_y )
        delete coord_y;
    
}