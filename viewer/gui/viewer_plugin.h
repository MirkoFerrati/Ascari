#ifndef VIEWER_PLUGIN_H
#define VIEWER_PLUGIN_H
#include <QPainter>
#include "Agent.h"
#include <assert.h>
class viewer_plugin
{
public:
  
  viewer_plugin()
  {
   setAgentShape();
   setAgentSize();
  }
  void setfather ( void* father )
    {
        assert ( father );
        this->father=father;
    };
  virtual void paintAgents(QPainter &painter,const std::map<std::string,Agent>& agents)
  {
  for ( std::map<std::string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
    {
          painter.save();
        painter.setBrush ( QColor ( "red" ) );
        painter.translate ( it->second.x,it->second.y );
        //TODO: Pessimo: lo zero degli angoli parte dall'asse y invece che da x
        double tmp=it->second.angle;
        while ( tmp>M_PI )
            tmp=tmp-2*M_PI;
        painter.rotate ( ( tmp*180/M_PI )-90 );

			    painter.scale(agentsize,agentsize);
            painter.drawConvexPolygon(agentshape);
				painter.restore();
				painter.save();
				painter.scale(painter.fontMetrics().height()/70.0,-painter.fontMetrics().height()/70.0);
				painter.drawText(0,0,QString(it->first.substr(6).c_str()));
				
        painter.restore();
      
    }
  };
  virtual void paintBackground(QPainter &painter){};
  virtual void timerEvent(){};
  virtual void init(std::string){};
  virtual void keypress(){};
  virtual void setAgentShape(QPolygon shape)
  {
    agentshape=shape;
  }
  virtual void setAgentShape()
  {
   agentshape=QPolygon(QVector<QPoint>(
    {
        QPoint ( 2, -2 ),
        QPoint ( -2, -2 ),
        QPoint ( 0, 2 )
    })); 
  }
  virtual void setAgentSize(double size=2.0)
  {
   agentsize=size; 
  }
  
  virtual ~viewer_plugin()
  {
  };
    void* father;
  
private:
    QPolygon agentshape;
    double agentsize;

};



#endif