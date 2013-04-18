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
#include <lemon/lgf_reader.h>
#include <map>
#include "../../shared/communication/global.h"
#define BORDER 0.3+0.2

using namespace std;


// Viewer::Viewer(const std::vector<char>& buffer,boost::asio::io_service& io_service, QWidget* parent,int view_type,std::string graphName)
//         : QWidget(parent)//,buffer(buffer)//,io_service(io_service)
// {
//   init(graphName);
// }

Viewer::Viewer ( const world_sim_packet& read, std::shared_ptr<std::mutex>& read_mutex, QWidget* parent, int view_type, std::string graphName ) :
    QWidget ( parent ),view_type ( view_type ),infos ( read ), mutex ( read_mutex )
{
    length=0;
    coord_x=0;
    coord_y=0;
    init ( graphName );
    monitor=true;
    time=0;
    started=false;
}



void Viewer::init ( std::string graphName )
{
    time=0;
    backImage="";
    scalingFactorX=10;
    scalingFactorY=10;
    translateX=0;
    translateY=0;
    maxX=0;
    maxY=0;
    minX=0;
    minY=0;
    this->view_type=view_type;
    if ( view_type==2 )
    {
        parse_graph ( graphName );
    }
    for ( lemon::SmartDigraph::NodeIt n ( graph ); n!=lemon::INVALID; ++n )
    {
        if ( maxX< ( *coord_x ) [n] ) maxX= ( *coord_x ) [n]*1.1;
        if ( maxY< ( *coord_y ) [n] ) maxX= ( *coord_y ) [n]*1.1;
        if ( minX> ( *coord_x ) [n] ) minX= ( *coord_x ) [n]*1.1;
        if ( minY< ( *coord_y ) [n] ) minY= ( *coord_y ) [n]*1.1;

    }
    
    for (lemon::SmartDigraph::NodeIt n(graph);n!=lemon::INVALID;++n)
	{
		if (maxX<(*coord_x)[n]) maxX=(*coord_x)[n]*1.1;
		if (maxY<(*coord_y)[n]) maxX=(*coord_y)[n]*1.1;
		if (minX>(*coord_x)[n]) minX=(*coord_x)[n]*1.1;
		if (minY<(*coord_y)[n]) minY=(*coord_y)[n]*1.1;
		
	}
	setScalingAndTranslateFactor(0,0,0,0);	

}

void Viewer::setMonitor ( std::map< std::string, monitor_packet >* monitor_read, shared_ptr< std::mutex > monitor_read_mutex )
{
    this->monitor_read=monitor_read;
    this->monitor_read_mutex=monitor_read_mutex;
}


void Viewer::parse_graph ( std::string graphName )
{
    coord_x= new lemon::SmartDigraph::NodeMap<int> ( graph );
    coord_y= new lemon::SmartDigraph::NodeMap<int> ( graph );
    length= new lemon::SmartDigraph::ArcMap<int> ( graph );

    try
    {
        lemon::digraphReader ( graph, graphName ). // read the directed graph into g
        nodeMap ( "coordinates_x", *coord_x ).	//read the coordinates of nodes
        nodeMap ( "coordinates_y", *coord_y ).	//read the coordinates of nodes
        arcMap ( "length", *length ).    // read the 'capacity' arc map into cap
        run();
    }
    catch ( lemon::Exception& er )   // check if there was any error
    {
    }

    std::cout << "A digraph is read from "<<graphName << std::endl;
    std::cout << "Number of nodes: " << lemon::countNodes ( graph ) << std::endl;
    std::cout << "Number of arcs: " << lemon::countArcs ( graph ) << std::endl;
}


void Viewer::setScalingAndTranslateFactor ( double maxX, double minX, double maxY, double minY )
{
    if ( this->maxX<maxX )
        this->maxX=maxX+abs ( maxX ) *BORDER;
    if ( this->minX>minX )
        this->minX=minX-abs ( minX ) *BORDER;
    if ( this->minY>minY )
        this->minY=minY-abs ( minY ) *BORDER;
    if ( this->maxY<maxY )
        this->maxY=maxY+abs ( maxY ) *BORDER;
    setScalingFactor ( this->maxX-this->minX,this->maxY-this->minY );
    setTranslateFactor ( ( this->maxX+this->minX ) /2, ( this->maxY+this->minY ) /2 );
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

//written by Alessandro Settimi
void Viewer::initialize_tasks(const std::map<std::string,task_assignment_task>& obj_tasks)
{
	*tasks = obj_tasks;
}
//written by Alessandro Settimi

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
//   	std::cout<<"distruttore di viewer chiamato dopo il salvataggio dei setting"<<std::endl;

    if ( length )
        delete length;
    if ( coord_x )
        delete coord_x;
    if ( coord_y )
        delete coord_y;

}

void Viewer::drawArrow(int x1,int y1, int x2, int y2, double sze, QPainter* painter )
{
    QPoint pd, pa, pb;
    double tangent;
    /* in order to get the arrowhead the correct way round, 
     * we reverse the gradient. 
     * This is equivalent to reversing the geometry below...
     */
    pd.setX(x2-x1);
    pd.setY(y2 - y1);
    
    if (pd.x() == 0 && pd.y() == 0)
        return;
    tangent = atan2 ((double) pd.y(), (double) pd.x());
    pa.setX(sze * cos (tangent + M_PI / 7) + x1);
    pa.setY(sze * sin (tangent + M_PI / 7) + y1);
    pb.setX(sze * cos (tangent - M_PI / 7) + x1);
    pb.setY(sze * sin (tangent - M_PI / 7) + y1);
    //-- connect the dots...
    
    painter->drawLine( pa.x(), pa.y(), x1, y1 );
    painter->drawLine( pb.x(), pb.y(), x1, y1 );
    
}

void Viewer::paintEvent ( QPaintEvent */*event*/ )
{
    static const QPoint hourHand[3] =
    {
        QPoint ( 2, -2 ),
        QPoint ( -2, -2 ),
        QPoint ( 0, 2 )
    };
    QColor hourColor ( 127, 0, 127 );

    double sidex=width();
    double sidey=height();
    QPainter painter ( this );

    if ( backImage.compare ( "" ) )
    {
        painter.drawPixmap ( 0,0,sidex,sidey,pixmap );
    }
    painter.setBrush ( Qt::white );
    painter.drawRect ( 0,0,10000,10000 );
    if ( view_type==1 )
    {
        painter.save();
        painter.setBrush ( QColor ( "lightgreen" ) );
        painter.drawRect ( 0,0,sidex,sidey );
        painter.restore();


        painter.save();
        painter.setBrush ( hourColor );
        painter.translate ( sidex/2,sidey/2 );
        painter.scale ( sidex/scalingFactorX,-sidey/scalingFactorY );
        painter.setBrush ( QColor ( "green" ) );
        painter.translate ( -translateX,-translateY );
        painter.rotate ( 45 );
        painter.drawRect ( 0,0,20,20 );
        painter.restore();
    }

    if ( view_type==2 )
    {
        painter.save();
        painter.setBrush ( QColor ( "lightgreen" ) );
        painter.translate ( sidex/2,sidey/2 );
        painter.scale ( sidex/scalingFactorX,-sidey/scalingFactorY );
        painter.translate ( -translateX,-translateY );

        for ( lemon::SmartDigraph::NodeIt n ( graph ); n!=lemon::INVALID; ++n )
        {
            painter.save();
            painter.translate ( ( *coord_x ) [n], ( *coord_y ) [n] );
            painter.drawEllipse ( QPoint ( 0,0 ),1,1 );
            painter.scale ( painter.fontMetrics().height() /20.0,-painter.fontMetrics().height() /20.0 );
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

    painter.save();
    QFont f = painter.font();
    f.setPointSizeF ( height() /25.0 );
    painter.setFont ( f );
    painter.setPen ( QColor ( "blue" ) );
    painter.drawText ( width() /2,1.1*painter.fontMetrics().height(), QString("").setNum(time) );
    painter.restore();

    painter.setBrush ( hourColor );
    painter.translate ( sidex/2,sidey/2 );
    painter.scale ( sidex/scalingFactorX,-sidey/scalingFactorY );
    painter.translate ( -translateX,-translateY );
    if ( view_type==1 )
    {
        painter.save();
        painter.translate ( 0,0 );
        painter.rotate ( 45 );
        painter.setBrush ( QColor ( "white" ) );
        painter.drawRect ( -1,-1,2,2 );
        painter.restore();

        painter.save();
        painter.setBrush ( QColor ( "red" ) );
        painter.translate ( 0,5 );
        painter.rotate ( 180 );
        painter.scale ( ( scalingFactorX*3.0/sidex ), ( scalingFactorY*3.0/sidey ) );
        painter.drawConvexPolygon ( hourHand,3 );
        painter.restore();

        painter.save();
        painter.setBrush ( QColor ( "white" ) );
        painter.translate ( 10,10 );
        painter.rotate ( 45 );
        painter.scale ( ( scalingFactorX*4.0/sidex ), ( scalingFactorY*4.0/sidey ) );
        painter.drawRect ( -2,-2,5,5 );
        painter.restore();

        painter.save();
        painter.setBrush ( QColor ( "white" ) );
        painter.translate ( -10,10 );
        painter.rotate ( 45 );
        painter.scale ( ( scalingFactorX*4.0/sidex ), ( scalingFactorY*4.0/sidey ) );
        painter.drawRect ( -2,-2,5,5 );
        painter.restore();

        painter.save();
        painter.setBrush ( QColor ( "white" ) );
        painter.translate ( 0,20 );
        painter.rotate ( 45 );
        painter.scale ( ( scalingFactorX*4.0/sidex ), ( scalingFactorY*4.0/sidey ) );
        painter.drawRect ( -2,-2,5,5 );
        painter.restore();
    }
    
    
    //written by Alessandro Settimi
    if (view_type==4)
    { 
      
	std::stringstream app(std::stringstream::out);
	app.str("");
	
	for (auto i=tasks->begin(); i!=tasks->end(); ++i)
	{
	  
		auto task = reinterpret_cast<const task_assignment_namespace::task*>(i->second.getState());

		painter.save();
		painter.translate(task->task_position[0],task->task_position[1]);
		
		if (task->task_type == 0)
		{
			painter.setBrush(QColor("lightgreen"));
		}
		else			
		{
			painter.setBrush(QColor("cyan"));
		}
		
		painter.drawRect(-1,-1,2,2);
		painter.restore();
		
		
		if (task->executing)
		{
			app <<  task->id.c_str() << " (" << ((task->task_execution_time-(time-task->time) >= 0)? task->task_execution_time-(time-task->time):(0)) << ")";
		}
		
		if(task->time!=0)
		{
			if(task->task_type!=0)
			{
			    if(task->task_type!=2)
			    {  
				if(time-task->time > task->task_execution_time)
				{
					painter.save();
					painter.translate(task->task_position[0],task->task_position[1]);
					
					painter.setBrush(QColor("white"));
					
					painter.drawRect(-1,-1,2,2);
					painter.restore();
				}
			    }
			    else if (!task->executing && time-task->time < task->period + task->task_execution_time)
			    {
				    painter.save();
				    painter.translate(task->task_position[0],task->task_position[1]);
				    
				    painter.setBrush(QColor("white"));
				    
				    painter.drawRect(-1,-1,2,2);
				    painter.restore();
			    }
			}
		}
		
		if(app.str()=="")
		{
			app << task->id.c_str() << " (" << task->task_execution_time << ")";
		}
		
		painter.save();
		painter.translate(task->task_position[0],task->task_position[1]);
		painter.setBrush(QColor("black"));
		painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.0);
		painter.drawText(0,75,QString(app.str().c_str()));
		painter.restore();
		
		app.str("");
		
		if (task->task_deadline != 0)
		{
			app << "[" << (((task->task_deadline-time)>=0)?(task->task_deadline-time):(0)) << "]";
		  
			painter.save();
			painter.translate(task->task_position[0],task->task_position[1]);
			painter.setPen(QColor("red"));
			painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.0);
			painter.drawText(0,-65,QString(app.str().c_str()));
			painter.restore();
			
			app.str("");
		}
	}
    }
    
    if(!agents.empty() && (!started || old_time>time))
    {
	      for ( map<string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
	      {
			std::vector<double> app;
	      
			app.push_back(it->second.x);
			app.push_back(it->second.y);
			
			initial_pos[it->first]=app;
	      }
	      
	      started=true;
    }
    
    old_time=time;
     //written by Alessandro Settimi 
    
    for ( map<string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
    { 
      
      
        painter.save();
        painter.setBrush ( QColor ( "red" ) );
        painter.translate ( it->second.x,it->second.y );
        //TODO: Pessimo: lo zero degli angoli parte dall'asse y invece che da x
        double tmp=it->second.angle;
        while ( tmp>M_PI )
            tmp=tmp-2*M_PI;
        painter.rotate ( ( tmp*180/M_PI )-90 );

//         painter.scale((scalingFactorX*3.0/sidex),(scalingFactorY*3.0/sidey));
        if ( it->first.compare ( "BALL" ) ==0 )
        {
            painter.setBrush ( QColor ( "white" ) );
            painter.drawEllipse ( 0,0,5,5 );
        }
        else
        {
			if (view_type==4)//written by Alessandro Settimi
			{
			    painter.scale(0.2,0.2);
			}
			else
			{
			    painter.scale(2,2);
			}
            painter.drawConvexPolygon(hourHand, 3);
			if (view_type==2 || view_type==5)
			{
				painter.save();
				painter.scale((scalingFactorX*3.0/sidex),(scalingFactorY*3.0/sidey));
				painter.scale(painter.fontMetrics().height()/70.0,-painter.fontMetrics().height()/70.0);
				painter.drawText(0,0,QString(it->first.substr(6).c_str()));
				painter.restore();
			}
			
			if (view_type==4)
			{
				painter.save();
				painter.scale((scalingFactorX*3.0/sidex),(scalingFactorY*3.0/sidey));
				painter.scale(painter.fontMetrics().height()/10.0,-painter.fontMetrics().height()/10.0);
				painter.drawText(0,0,QString(it->first.substr(6).c_str()));
				painter.restore();
			}

        }
        painter.restore();
	
	if (view_type==4 && time>0)
	{ 
		painter.save();
		painter.translate(initial_pos.at(it->first).at(0),initial_pos.at(it->first).at(1));
		painter.drawArc(-1,-1,2,2,0,16*360);
		painter.restore();
		
		std::stringstream tm(std::stringstream::out);
		tm << (int)it->second.charge << "%";
		
		painter.save();
		painter.translate(it->second.x,it->second.y);
		painter.setPen(QColor("green"));
		painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.0);
		painter.drawText(-20,-20,QString(tm.str().c_str()));
		painter.restore();
		
		//per la scia

		positions[it->first].set_capacity(600);
		positions[it->first].push_back(it->second.x);
		positions[it->first].push_back(it->second.y);
		
		for (unsigned int i=0;i<positions[it->first].size();i=i+2)
		{
			painter.save();
			painter.translate(positions[it->first].at(i),positions[it->first].at(i+1));
			painter.setPen(QColor("black"));
			painter.drawPoint(0,0);
			painter.restore();
		}
	}
				
	    if ( view_type==5 && monitor )
            {
                painter.save();
		painter.setBrush(Qt::NoBrush);

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
                            painter.drawLine (it->second.x , it->second.y,agents.at ( target.agent_id ).x,agents.at ( target.agent_id ).y );
			     drawArrow (agents.at ( target.agent_id ).x,agents.at ( target.agent_id ).y,it->second.x , it->second.y,20,&painter );
			}
                    }
                }
                monitor_read_mutex->unlock();
                painter.restore();
            }
    }

}


void Viewer::timerEvent ( QTimerEvent */*event*/ )
{
    //cout<<"timer event"<<endl;
    /*    try
        {

            io_service.poll();
            if (buffer.size()>0 )
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
            //throw "Problema nella decodifica di un pacchetto";
        }
    */
    mutex->lock();
    for ( map<string, agent_state_packet>::const_iterator it=infos.state_agents.internal_map.begin(); it!=infos.state_agents.internal_map.end(); ++it )
    {
        agents[it->first].translate ( (it->second) );
        //setScalingFactor(agents[it->first].getMaxX()-agents[it->first].getMinX(),agents[it->first].getMaxY()-agents[it->first].getMinY());
        //setTranslateFactor((agents[it->first].getMaxX()+agents[it->first].getMinX())/2,(agents[it->first].getMaxY()+agents[it->first].getMinY())/2);
        setScalingAndTranslateFactor ( agents[it->first].getMaxX(),agents[it->first].getMinX(),agents[it->first].getMaxY(),agents[it->first].getMinY() );
    }
    
    //written by Alessandro Settimi
    if (view_type==4)
    {
	for(auto i=tasks->begin(); i!=tasks->end(); ++i)
	{
	    auto task = reinterpret_cast<const task_assignment_namespace::task*>(i->second.getState());
	    setScalingAndTranslateFactor(task->task_position[0],task->task_position[0]-2,task->task_position[1],task->task_position[1]-2);
	}
    }
    //written by Alessandro Settimi
    
    time=infos.time;
    mutex->unlock();
    repaint();

}

void Viewer::keyPressEvent ( QKeyEvent *event )
{
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
    case Qt::Key_M:
    {
        monitor=!monitor;
    }
    break;
    default:
        QWidget::keyPressEvent ( event );
    }
}

void Viewer::start()
{

    timerId = startTimer ( 10 );
    cout<<"timer started"<<endl;
    repaint();

}

void Viewer::pause()
{
    throw "not implemented";
}

