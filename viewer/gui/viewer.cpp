#include "viewer.h"
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <math.h>
#include <QtCore/QLocale>
#include <QSettings>
#include <QtGui/QWidget>

#include "debug_constants.h"
#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <boost/lexical_cast.hpp>
#include <vector>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iomanip>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <map>
#include "../../shared/communication/global.h"
#include <define.h>
#define VIEWER_BORDER atof(CONFIG.getValue("VIEWER_BORDER").c_str())

using namespace std;


Viewer::Viewer ( const world_sim_packet& read, std::shared_ptr<std::mutex>& read_mutex, QWidget* parent) :
    QGraphicsView ( parent ),infos ( read ), mutex ( read_mutex )
{
    simulation_time=0;
    agentshape=QPolygon(QVector<QPoint>({ QPoint ( 20, -20 ),QPoint ( -20, -20 ),QPoint ( 0, 20 )})); 
    
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    
    //Set-up the scene
     Scene = new QGraphicsScene(this);
    setScene(Scene);
    
    //Populate the scene
    for(int x = 0; x < 1000; x = x + 25) {
        for(int y = 0; y < 1000; y = y + 25) {
            
            if(x % 100 == 0 && y % 100 == 0) {
                Scene->addRect(x, y, 2, 2);
                
                QString pointString;
                QTextStream stream(&pointString);
                stream << "(" << x << "," << y << ")";
                QGraphicsTextItem* item = Scene->addText(pointString);
                item->setPos(x, y);
            } else {
                Scene->addRect(x, y, 1, 1);
            }
        }
    }
    
    //Set-up the view
    setSceneRect(0, 0, 1000, 1000);
    
    //Use ScrollHand Drag Mode to enable Panning
    setDragMode(ScrollHandDrag);
}

void Viewer::wheelEvent(QWheelEvent* event)
{        
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        
        // Scale the view / do the zoom
        double scaleFactor = 1.15;
        if(event->delta() > 0) {
            // Zoom in
            scale(scaleFactor, scaleFactor);
        } else {
            // Zooming out
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }       
        // Don't call superclass handler here
        // as wheel is normally used for moving scrollbars
        // QGraphicsView::wheelEvent(event);
}


void Viewer::addPlugin ( abstract_viewer_plugin* plugin )
{
  assert(plugin);
  plugins.push_back(plugin);
}



void Viewer::init ( std::string filename )
{
    simulation_time=0;
    backImage="";
    scalingFactorX=10;
    scalingFactorY=10;
    translateX=0;
    translateY=0;
    maxX=-500000;
    maxY=-500000;
    minX=500000;
    minY=500000;
    
    for (auto plugin:plugins)
      plugin->init();

   // setScalingAndTranslateFactor(0,0,0,0);	

}

void Viewer::setScalingAndTranslateFactor ( double maxX, double minX, double maxY, double minY )
{
    if ( this->maxX<maxX )
        this->maxX=maxX +(maxX+minX)/2.0*VIEWER_BORDER;
    if ( this->minX>minX )
		this->minX=minX -(maxX+minX)/2.0*VIEWER_BORDER;
    if ( this->minY>minY )
		this->minY=minY -(maxY+minY)/2.0*VIEWER_BORDER;
    if ( this->maxY<maxY )
		this->maxY=maxY +(maxY+minY)/2.0*VIEWER_BORDER;
	
	setScalingFactor ( this->maxX-this->minX,this->maxY-this->minY );
	setTranslateFactor ( ( this->maxX+this->minX ) /2.0, ( this->maxY+this->minY ) /2.0 );

	
}


void Viewer::setScalingFactor ( double scalingFactorX,double scalingFactorY )
{
    if ( this->scalingFactorX<scalingFactorX )
        this->scalingFactorX=scalingFactorX;
    if ( this->scalingFactorY<scalingFactorY )
        this->scalingFactorY=scalingFactorY;
}

void Viewer::setTranslateFactor ( double translateX, double translateY )
{
    this->translateX=translateX;
    this->translateY=translateY;
}


void Viewer::setBackImage ( string path )
{
    this->backImage=path;
    if ( backImage.compare ( "" ) )
    {
        immagine=QImage ( QString ( backImage.c_str() ) );
        immagine=immagine.scaled ( QSize ( width(),height() ),
                                   Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation );
    }
    pixmap.convertFromImage ( immagine );
}

 void Viewer::closeEvent(QCloseEvent *event)
 {
QSettings settings("K2BRobotics","Viewer");
      settings.setValue("mainWindowGeometry", saveGeometry());
     QWidget::closeEvent(event);

 }


Viewer::~Viewer()
{
  {
  QSettings settings;
      settings.setValue("mainWindowGeometry", this->saveGeometry());
  }
}

void Viewer::paintTextPoint(QPainter *painter,double x,double y)
{
	painter->save();
	painter->setPen(QColor("blue"));
	QFont f = painter->font();
	f.setPointSizeF ( 20 );
	painter->setFont ( f );
	QString s;
	s.append(QString("").setNum(x*0.9));
	s.append(",");
	s.append(QString("").setNum(y*0.9));
	painter->translate(x*0.9,y*0.9);
	painter->scale(1,-1);
	painter->drawText(0,0,s);
	painter->restore();
	
}

void Viewer::paintAgents(const std::map<std::string,Agent>& agents)
{       
    for ( std::map<std::string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
    {
        std::cout<<it->first<<std::endl;
        /*	painter.save();
	painter.setBrush ( QColor ( "red" ) );
	painter.translate ( it->second.x,it->second.y );
*/	//lo zero degli angoli parte dall'asse y invece che da x
	double tmp=it->second.angle;
	while ( tmp>M_PI )
		tmp=tmp-2*M_PI;
        it->second.shape->setPos(it->second.x,it->second.y);
        it->second.shape->setRotation(tmp*180/M_PI-90);
        /*	painter.rotate ( ( tmp*180/M_PI )-90 );
	
	painter.scale(3,3);
	painter.drawConvexPolygon(agentshape);
	painter.restore();
	painter.save();
	painter.translate ( it->second.x,it->second.y );
	painter.scale(painter.fontMetrics().height()/100,-painter.fontMetrics().height()/100);
	painter.drawText(0,0,QString(it->first.substr(6).c_str()));
	
	painter.restore();
*/

    }
}

void Viewer::paintEvent ( QPaintEvent *event )
{
    double sidex=width();
    double sidey=height();
    //QPainter painter ( this );
    /*painter.save();
    painter.translate ( sidex/2,sidey/2 );
    painter.scale ( sidex/scalingFactorX,-sidey/scalingFactorY );
    painter.translate ( -translateX,-translateY );
// 	paintTextPoint(&painter,maxX,maxY);
// 	paintTextPoint(&painter,maxX,minY);
// 	paintTextPoint(&painter,minX,maxY);
// 	paintTextPoint(&painter,minX,minY);


    if ( backImage.compare ( "" ) )
    {
        painter.drawPixmap ( 0,0,sidex,sidey,pixmap );
    }

    if(plugins.size()==0)
    {*/
	paintAgents(agents);
	
	/*painter.save();
	QFont f = painter.font();
	f.setPointSizeF (  std::max(height() /2500.0,0.04 ));
	painter.setFont ( f );
	painter.setPen ( QColor ( "blue" ) );
	painter.translate(translateX,maxY-1.1*painter.fontMetrics().height());
	painter.scale(1,-1);
	painter.drawText (0,0, QString("").setNum(simulation_time) );
	painter.restore();*/
   /* }
  
    for (auto plugin:plugins)
    {
	painter.save();
	plugin->paintBackground(painter);
	painter.restore();
    }
    for (auto plugin:plugins)
    {
	painter.save();
	plugin->paintAgents(painter,agents);
	painter.restore();	
    }
	
   
    painter.restore();*/
  QGraphicsView::paintEvent(event);
}


void Viewer::timerEvent ( QTimerEvent */*event*/ )
{
    
    mutex->lock();
    for ( map<string, agent_state_packet>::const_iterator it=infos.state_agents.internal_map.begin(); it!=infos.state_agents.internal_map.end(); ++it )
    {
        agents[it->first].translate ( (it->second) );
        setScalingAndTranslateFactor ( agents[it->first].getMaxX(),agents[it->first].getMinX(),agents[it->first].getMaxY(),agents[it->first].getMinY() );
    }
    simulation_time=infos.time;
    //mutex->unlock();
        for ( std::map<std::string,Agent>::iterator it=agents.begin(); it!=agents.end(); ++it )
        {
            if (!it->second.created)
            {   
            QGraphicsPolygonItem *agent=Scene->addPolygon(agentshape);
            it->second.shape=agent;
            it->second.created=true;
            }
    }
    mutex->unlock();
    for (auto plugin:plugins)
    {
      plugin->timerEvent(mutex,infos);
    }
    
    repaint();

}

void Viewer::keyPressEvent ( QKeyEvent *event )
{
  
   for (auto plugin:plugins)
   {
     plugin->keypress();
   }
    switch ( event->key() )
    {

    case Qt::Key_P:
    {
        pause();
    }
    break;
    case Qt::Key_Escape:
    {
        qApp->exit();
    }
    break;
    case Qt::Key_Q:
    {
        qApp->exit();
    }
    break;
    default:
        QWidget::keyPressEvent ( event );
    }
}

void Viewer::start()
{

    timerId = startTimer ( 25 );
    cout<<"timer started"<<endl;
    repaint();

}

void Viewer::pause()
{
    throw "not implemented";
}

