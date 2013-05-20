#ifndef ABSTRACT_VIEWER_PLUGIN_H
#define ABSTRACT_VIEWER_PLUGIN_H


#include "HACK_KDEVELOP.h"

#include <QPainter>
#include "../viewer/gui/Agent.h"
#include <assert.h>
#include "types.h"
#include <types/world_sim_packet.h>
#include <mutex>
#include <memory>

class abstract_viewer_plugin
{
public:
	
	abstract_viewer_plugin()
	{
		setAgentShape();
		setAgentSize();
		setPainterScale();
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
			painter.translate ( it->second.x,it->second.y );
			painter.scale(painter.fontMetrics().height()/painterscale,-painter.fontMetrics().height()/painterscale);
			painter.drawText(0,0,QString(it->first.substr(6).c_str()));
			
			painter.restore();
			
		}
	};
	virtual void paintBackground(QPainter &/*painter*/){};
	virtual void timerEvent(std::shared_ptr<std::mutex>&,const world_sim_packet&){};
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
	
	virtual void setPainterScale(double scale=70.0)
	{
		painterscale=scale;
	}
	virtual ~abstract_viewer_plugin()
	{
	};
	void* father;
	
private:
	QPolygon agentshape;
	double agentsize;
	double painterscale;
	
};


#endif // ABSTRACT_VIEWER_PLUGIN_H
