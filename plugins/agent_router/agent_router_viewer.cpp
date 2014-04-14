#include "agent_router_viewer.h"
#include "agent_router_parsed_world.h"
#include "viewer.h"
#include <lemon/lgf_reader.h>
#include <logog.hpp>

#include "../HACK_KDEVELOP.h"
#include "agent_router_plugin.h"

using namespace std;

agent_router_viewer::agent_router_viewer()
{
  father=0;
     length=0;
    coord_x=0;
    coord_y=0;
}

agent_router_viewer::agent_router_viewer ( Parsed_World* world )
{
  father=0;
  length=0;
  coord_x=0;
  coord_y=0;
  auto temp=reinterpret_cast<agent_router_parsed_world*>(world->parsed_items_from_plugins[AGENT_ROUTER_NAME]);
  graphName=boost::to_lower_copy(temp->graphName);
}



void agent_router_viewer::init(  )
{
  assert(father);
  Viewer* temp_father=reinterpret_cast<Viewer*>(father);
  try{
    parseGraph (graphName);
  }
  catch (const char* ex)
  {
    ERR("impossibile parsare il file %s: %s",graphName.c_str(),ex);
    assert(0);
  }
	lemon::SmartDigraph::NodeIt n ( graph );
// 	double maxx=( *coord_x ) [n],maxy=( *coord_y ) [n],minx=( *coord_x ) [n],miny=( *coord_y ) [n];
//     for ( lemon::SmartDigraph::NodeIt n ( graph ); n!=lemon::INVALID; ++n )
//     {
//         if ( maxx< ( *coord_x ) [n] ) maxx= ( *coord_x ) [n];
//         if ( maxy< ( *coord_y ) [n] ) maxy= ( *coord_y ) [n];
//         if ( minx> ( *coord_x ) [n] ) minx= ( *coord_x ) [n];
//         if ( miny> ( *coord_y ) [n] ) miny= ( *coord_y ) [n];
//     }
//     temp_father->setScalingAndTranslateFactor(maxx,minx,maxy,miny);
}


void agent_router_viewer::paintBackground(QGraphicsScene* scene)
{
  assert(father);
  if (!paintedBackground)
  {
      paintedBackground=true;
        QPen temp;
        QBrush brush;
        brush.setColor ( QColor ( "lightgreen" ) );
        
        for ( lemon::SmartDigraph::NodeIt n ( graph ); n!=lemon::INVALID; ++n )
        {
            scene->addEllipse(( *coord_x ) [n], ( *coord_y ) [n],4,4,temp,brush);
         
            auto item=scene->addText (QString ( "" ).setNum ( graph.id ( n ) ) );
            item->setPos(( *coord_x ) [n], ( *coord_y ) [n]);
            item->scale(1,-1);
        }

        temp.setBrush(QColor("black"));
        for ( lemon::SmartDigraph::ArcIt a ( graph ); a!=lemon::INVALID; ++a )
        {
            scene->addLine(( *coord_x ) [graph.source ( a )], ( *coord_y ) [graph.source ( a )], ( *coord_x ) [graph.target ( a )], ( *coord_y ) [graph.target ( a )],temp );
        }
  }
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
      ERR("%s",er.what());
      throw "error while parsing graph";
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
