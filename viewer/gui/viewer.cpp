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
    agentshape=QPolygon(QVector<QPoint>( { QPoint ( -10, 10 ),QPoint ( -10, -10 ),QPoint ( 30, 0 )}));

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //Set-up the scene
    Scene = new QGraphicsScene(this);
    setScene(Scene);
    scale(1,-1);
    
    /*
     * Use the following set of instruction to see the coordinate system of viewer.
     */
    /*for (double theta=0;theta<M_PI;theta=theta+0.4)
    {
        auto line=Scene->addLine(0,0,50,0);
        line->setRotation(theta*180.0/M_PI);
        auto item=Scene->addText(QString("").setNum(theta));
        item->setPos(80*cos(theta)-10,80*sin(theta)+10);
        item->scale(1,-1);
    }
    
    //fake agent with theta=0.4
    QGraphicsPolygonItem *agent=Scene->addPolygon(agentshape);
    QBrush b;
    b.setColor(QColor("red"));
    b.setStyle( Qt::SolidPattern);
    agent->setBrush(b); 
    double tmp=0.4;
    agent->setPos(0,0);
    agent->setRotation(tmp*180.0/M_PI);
    
    for(int x = 0; x < 1000; x = x + 250) {
        for(int y = 0; y < 1000; y = y + 250) {

            if(x % 100 == 0 && y % 100 == 0) {
                Scene->addRect(x, y, 2, 2);

                QString pointString;
                QTextStream stream(&pointString);
                stream << "(" << x << "," << y << ")";
                QGraphicsTextItem* item = Scene->addText(pointString);
                item->scale(1,-1);
                item->setPos(x, y);
            } else {
                Scene->addRect(x, y, 1, 1);
            }
        }
    }
    /**/
    clock=Scene->addText("");
    clock->scale(1,-1);
    clock->setFlag(QGraphicsItem::ItemIsMovable,true);
    clock->setFlag(QGraphicsItem::ItemIsSelectable,true);


    //Set-up the view
//    setSceneRect(-500, -500, 1000, 1000); if this instruction is not called, the sceneRect will include 

    //Use ScrollHand Drag Mode to enable Panning
    setDragMode(ScrollHandDrag);
}

void Viewer::wheelEvent(QWheelEvent* event)
{
    bool selected=false;
for (auto item:Scene->selectedItems())
    {
        selected=true;
        if (item==clock)
        {
            QFont temp=clock->font();
            temp.setPointSize(temp.pointSize()+((event->delta()>0)*2-1));
            clock->setFont(temp);
        }
        else if (item->data(0)==TYPE_AGENT)
        {
            item->setScale(item->scale()*((event->delta()>0)*0.2+0.9));
        }
        else if (item->data(0)==TYPE_TEXT)
        {

        }
    }

    if (!selected)
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
for (auto plugin:plugins)
        plugin->init();

}

void Viewer::setScalingAndTranslateFactor ( double maxX, double minX, double maxY, double minY )
{
    setSceneRect(minX, minY, maxX-minX, maxY-minY);
}


void Viewer::setScalingFactor ( double scalingFactorX,double scalingFactorY )
{
    //scale(scalingFactorX, scalingFactorY);
}

void Viewer::setTranslateFactor ( double translateX, double translateY )
{

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
    QSettings settings("Ascari","Viewer");
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

void Viewer::paintEvent ( QPaintEvent *event )
{
    double sidex=width();
    double sidey=height();

    if(plugins.size()==0)
    {
        paintAgents(Scene,agents);
        clock->setPlainText(QString("").setNum(simulation_time));
    }

for (auto plugin:plugins)
    {
        plugin->paintBackground(Scene);
    }
for (auto plugin:plugins)
    {
        plugin->paintAgents(Scene,agents);
    }
    QGraphicsView::paintEvent(event);
}


void Viewer::timerEvent ( QTimerEvent */*event*/ )
{

    mutex->lock();
    for ( map<string, agent_state_packet>::const_iterator it=infos.state_agents.internal_map.begin(); it!=infos.state_agents.internal_map.end(); ++it )
    {
        agents[it->first].translate ( (it->second) );
        //setScalingAndTranslateFactor ( agents[it->first].getMaxX(),agents[it->first].getMinX(),agents[it->first].getMaxY(),agents[it->first].getMinY() );
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

    timerId = startTimer ( 40 );
    cout<<"timer started"<<endl;
    repaint();

}

void Viewer::pause()
{
    if (timerId)
    {
        killTimer(timerId);
        timerId=0;
    }
    else
        start();
}

