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
    QWidget ( parent ),infos ( read ), mutex ( read_mutex )
{
    simulation_time=0;
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

void Viewer::paintEvent ( QPaintEvent */*event*/ )
{
    double sidex=width();
    double sidey=height();
    QPainter painter ( this );
    painter.save();
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
    
    painter.save();
    QFont f = painter.font();
    f.setPointSizeF ( max(height() /25.0,4.0 ));
    painter.setFont ( f );
    painter.setPen ( QColor ( "blue" ) );
	painter.translate(translateX,maxY-1.1*painter.fontMetrics().height());
	painter.scale(1,-1);
    painter.drawText (0,0, QString("").setNum(simulation_time) );
    painter.restore();
    painter.restore();
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

