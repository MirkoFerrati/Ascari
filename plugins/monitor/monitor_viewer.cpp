#include "monitor_viewer.h"
#include "viewer.h"
#include <../communication/zmq_identifier_sniffer.hpp>
#include <../simulator/visibility/map2d.h>
#include "../../gml2lgf/arc.h"
#include "../plugins/abstract_viewer_plugin.h"
#include <mutex>
#include <memory>
#include <string>
#include <map>
#include <sstream>
monitor_viewer::monitor_viewer ( std::string mapfilename ) :mapfilename ( mapfilename )
{
    monitor=true;
    this->monitor_read_mutex=std::make_shared<std::mutex> ( new std::mutex() );
    map=0;
    identifier_sniffer.reset ( new zmq_identifier_sniffer ( monitor_read,monitor_read_mutex ) );
    identifier_sniffer->start_receiving();

    setPainterScale ( 25.0 );
}

monitor_viewer::monitor_viewer()
{
    monitor=true;
    this->monitor_read_mutex=std::make_shared<std::mutex> ( new std::mutex() );
    map=0;
    identifier_sniffer.reset ( new zmq_identifier_sniffer ( monitor_read,monitor_read_mutex ) );
    identifier_sniffer->start_receiving();

    setPainterScale ( 25.0 );
}

monitor_viewer::~monitor_viewer()
{
    if ( identifier_sniffer )
        identifier_sniffer->stop_receiving();
}


void monitor_viewer::drawArrow ( int x1,int y1, int x2, int y2, double sze, QPainter* painter )
{
    QPoint pd, pa, pb;
    double tangent;
    /* in order to get the arrowhead the correct way round,
     * we reverse the gradient.
     * This is equivalent to reversing the geometry below...
     */
    pd.setX ( x2-x1 );
    pd.setY ( y2 - y1 );

    if ( pd.x() == 0 && pd.y() == 0 )
        return;
    tangent = atan2 ( ( double ) pd.y(), ( double ) pd.x() );
    pa.setX ( sze * cos ( tangent + M_PI / 7 ) + x1 );
    pa.setY ( sze * sin ( tangent + M_PI / 7 ) + y1 );
    pb.setX ( sze * cos ( tangent - M_PI / 7 ) + x1 );
    pb.setY ( sze * sin ( tangent - M_PI / 7 ) + y1 );
    //-- connect the dots...

    painter->drawLine ( pa.x(), pa.y(), x1, y1 );
    painter->drawLine ( pb.x(), pb.y(), x1, y1 );

}

void monitor_viewer::paintBackground ( QPainter& painter )
{
    if ( !map )
        return;
    auto &nodes=map->getGraph().nodes;
    auto &arcs=map->getGraph().arcs;
    for ( unsigned int i=0; i<arcs.size(); i++ )
    {
        painter.drawLine ( nodes[arcs[i].first].x,nodes[arcs[i].first].y,nodes[arcs[i].second].x,nodes[arcs[i].second].y );
    }


}

void monitor_viewer::init (  )
{
    if ( mapfilename!="" )
    {
        map=new map2d ( mapfilename );


        assert ( father );
        Viewer* temp_father=reinterpret_cast<Viewer*> ( father );
        auto &nodes=map->getGraph().nodes;

        double maxx=nodes[0].x,maxy=nodes[0].y,minx=nodes[0].x,miny=nodes[0].y;
        for ( unsigned int i=0; i<nodes.size(); i++ )
        {
            if ( maxx< nodes[i].x ) maxx= nodes[i].x;
            if ( maxy< nodes[i].y ) maxy= nodes[i].y;
            if ( minx> nodes[i].x ) minx= nodes[i].x;
            if ( miny> nodes[i].y ) miny= nodes[i].y;
        }
        temp_father->setScalingAndTranslateFactor ( maxx,minx,maxy,miny );

    }
}


void monitor_viewer::paintAgents ( QPainter& painter, const std::map< std::string, Agent >& agents )
{
    abstract_viewer_plugin::paintAgents ( painter, agents );

    for ( std::map<std::string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
    {
        if ( monitor )
        {
            painter.save();
            painter.setBrush ( Qt::NoBrush );

            QPen pen;
            pen.setColor ( QColor ( "blue" ) );
            pen.setStyle ( Qt::DashLine );
            painter.setPen ( pen );
            //painter.drawEllipse(QPointF(it->second.x,it->second.y),RADIUS,RADIUS);

            monitor_read_mutex->lock();
            if ( monitor_read->count ( it->first ) )
            {
            for ( auto target:monitor_read->at ( it->first ).agents )
                {
                    if ( !target.ok )
                    {
                        painter.drawLine ( it->second.x , it->second.y,agents.at ( target.agent_id ).x,agents.at ( target.agent_id ).y );
                        drawArrow ( agents.at ( target.agent_id ).x,agents.at ( target.agent_id ).y,it->second.x , it->second.y,20,&painter );
                    }
                }
            }
            monitor_read_mutex->unlock();
            painter.restore();
        }


    }
}

