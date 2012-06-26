#include "viewer.h"
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <math.h>
#include <QtCore/QLocale>
#include <QtGui/QWidget>
#include "typedefs.h"
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

#define BORDER 0.3+0.2

Viewer::Viewer(const std::vector<char>& buffer,boost::asio::io_service& io_service, QWidget* parent)
        : QWidget(parent),buffer(buffer),io_service(io_service)
{
    time=0;
    backImage="";
    scalingFactorX=30;
    scalingFactorY=30;
    translateX=0;
    translateY=0;
    maxX=0;
    maxY=0;
    minX=0;
    minY=0;
}

using namespace std;

void Viewer::setScalingAndTranslateFactor(double maxX, double minX, double maxY, double minY)
{
    if (this->maxX<maxX)
        this->maxX=maxX+abs(maxX)*BORDER;
    if (this->minX>minX)
        this->minX=minX-abs(minX)*BORDER;
    if (this->minY>minY)
        this->minY=minY-abs(minY)*BORDER;
    if (this->maxY<maxY)
        this->maxY=maxY+abs(maxY)*BORDER;
    setScalingFactor(this->maxX-this->minX,this->maxY-this->minY);
    setTranslateFactor((this->maxX+this->minX)/2,(this->maxY+this->minY)/2);
}


void Viewer::setScalingFactor(double scalingFactorX,double scalingFactorY)
{
    if (this->scalingFactorX<scalingFactorX)
        this->scalingFactorX=scalingFactorX;
    if (this->scalingFactorY<scalingFactorY)
        this->scalingFactorY=scalingFactorY;
}

void Viewer::setTranslateFactor(double translateX, double translateY)
{
    this->translateX=translateX;
    this->translateY=translateY;
}


void Viewer::setBackImage(string path) {
    this->backImage=path;
    if (backImage.compare("")) {
        immagine=QImage(QString(backImage.c_str()));
        immagine=immagine.scaled(QSize(width(),height()),
                                 Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    }
    pixmap.convertFromImage(immagine);
}

Viewer::~Viewer() {

}

void Viewer::paintEvent(QPaintEvent *event)
{
    static const QPoint hourHand[3] = {
        QPoint(3, -3),
        QPoint(-3, -3),
        QPoint(0, 4)
    };
    QColor hourColor(127, 0, 127);

    double sidex=width();
    double sidey=height();
    QPainter painter(this);

    if (backImage.compare("")) {
        painter.drawPixmap(0,0,sidex,sidey,pixmap);
    }

    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();
    painter.setBrush(QColor("lightgreen"));
    painter.drawRect(0,0,sidex,sidey);
    painter.restore();


    painter.save();
	painter.setBrush(hourColor);
    painter.translate(sidex/2,sidey/2);
    painter.scale(sidex/scalingFactorX,-sidey/scalingFactorY);
    painter.setBrush(QColor("green"));
	painter.translate(-translateX,-translateY);
	painter.rotate(45);
    painter.drawRect(0,0,20,20);
    painter.restore();

	painter.save();
    QFont f = painter.font();
    f.setPointSizeF(height()/25.0);
    painter.setFont(f);
    painter.setPen(QColor("blue"));
    QString s;
    s.setNum(time);
    painter.drawText(width()/2,1.1*painter.fontMetrics().height(),s);
    painter.restore();

    painter.setBrush(hourColor);
    painter.translate(sidex/2,sidey/2);
    painter.scale(sidex/scalingFactorX,-sidey/scalingFactorY);
    painter.translate(-translateX,-translateY);
	
    painter.save();
    painter.translate(0,0);
    painter.rotate(45);
    painter.setBrush(QColor("white"));
    painter.drawRect(-1,-1,2,2);
    painter.restore();

    painter.save();
    painter.setBrush(Qt::GlobalColor::red);
    painter.translate(0,5);
    painter.rotate(180);
    painter.scale((scalingFactorX*3.0/sidex),(scalingFactorY*3.0/sidey));
    painter.drawConvexPolygon(hourHand,3);
    painter.restore();

    painter.save();
    painter.setBrush(Qt::GlobalColor::white);
    painter.translate(10,10);
    painter.rotate(45);
    painter.scale((scalingFactorX*4.0/sidex),(scalingFactorY*4.0/sidey));
    painter.drawRect(-2,-2,5,5);
    painter.restore();

    painter.save();
    painter.setBrush(Qt::GlobalColor::white);
    painter.translate(-10,10);
    painter.rotate(45);
    painter.scale((scalingFactorX*4.0/sidex),(scalingFactorY*4.0/sidey));
    painter.drawRect(-2,-2,5,5);
    painter.restore();

    painter.save();
    painter.setBrush(Qt::GlobalColor::white);
    painter.translate(0,20);
    painter.rotate(45);
    painter.scale((scalingFactorX*4.0/sidex),(scalingFactorY*4.0/sidey));
    painter.drawRect(-2,-2,5,5);
    painter.restore();

    
    for (map<string,Agent>::const_iterator it=agents.begin();it!=agents.end();it++)
    {
        painter.save();
        painter.setBrush(Qt::GlobalColor::red);
        painter.translate(it->second.x,it->second.y);
        //TODO: Pessimo: lo zero degli angoli parte dall'asse y invece che da x
        double tmp=it->second.angle;
        while (tmp>M_PI)
            tmp=tmp-2*M_PI;
        painter.rotate((tmp*180/M_PI)-90);

        painter.scale((scalingFactorX*3.0/sidex),(scalingFactorY*3.0/sidey));
        if (it->first.compare("BALL")==0)
        {
            painter.setBrush(QColor("white"));
            painter.drawEllipse(0,0,5,5);
        }
        else
        {
            painter.drawConvexPolygon(hourHand, 3);
        }
        painter.restore();
    }

}


void Viewer::timerEvent(QTimerEvent *event)
{
    try
    {
        io_service.poll();
        if (buffer.size()>0)
        {
            std::string archive_data(&buffer[header_length], buffer.size()-header_length);
            std::istringstream archive_stream(archive_data);
            boost::archive::text_iarchive archive(archive_stream);
            archive >> infos;
        }
    }
    catch (std::exception& e)
    {
        // Unable to decode data.
        boost::system::error_code error(boost::asio::error::invalid_argument);
        throw "Problema nella decodifica di un pacchetto";
    }

    for (map<string, agent_state_packet>::const_iterator it=infos.state_agents.internal_map.begin();it!=infos.state_agents.internal_map.end();it++)
    {
        agents[it->first].translate(it->second);
        //setScalingFactor(agents[it->first].getMaxX()-agents[it->first].getMinX(),agents[it->first].getMaxY()-agents[it->first].getMinY());
        //setTranslateFactor((agents[it->first].getMaxX()+agents[it->first].getMinX())/2,(agents[it->first].getMaxY()+agents[it->first].getMinY())/2);
        setScalingAndTranslateFactor(agents[it->first].getMaxX(),agents[it->first].getMinX(),agents[it->first].getMaxY(),agents[it->first].getMinY());
    }
    repaint();
    time++;
}

void Viewer::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {

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
        QWidget::keyPressEvent(event);
    }
}

void Viewer::start()
{

    timerId = startTimer(10);

}

void Viewer::pause()
{
    throw "not implemented";
}

