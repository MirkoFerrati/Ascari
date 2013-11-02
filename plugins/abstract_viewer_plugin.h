#ifndef ABSTRACT_VIEWER_PLUGIN_H
#define ABSTRACT_VIEWER_PLUGIN_H


#include "HACK_KDEVELOP.h"
#include <qt4/QtGui/QPainter>

#include "../viewer/gui/Agent.h"
#include <assert.h>
#include "types.h"
#include <types/world_sim_packet.h>
#include <yaml_parser.h>
#include <mutex>
#include <memory>
#include <qt4/QtGui/QGraphicsScene>
#include <qt4/QtGui/QGraphicsItem>

#define TYPE_AGENT 1234
#define TYPE_TEXT 5678

class abstract_viewer_plugin
{
public:
	
	abstract_viewer_plugin()
	{
            
	}
	void setfather ( void* father )
	{
		assert ( father );
		this->father=father;
	};
	
	virtual void paintAgents(QGraphicsScene* Scene, std::map<std::string,Agent>& agents)
	{
            for ( std::map<std::string,Agent>::iterator it=agents.begin(); it!=agents.end(); ++it )
            {
                if (!it->second.created)
                {
                    QGraphicsPolygonItem *agent=Scene->addPolygon(agentshape);
                    std::cout<<"creato nuovo agente"<<std::endl;
                    QBrush b;
                    b.setColor(QColor("red"));
                    b.setStyle( Qt::SolidPattern);
                    agent->setBrush(b);
                    agent->setData(0,TYPE_AGENT);
                    agent->setFlag(QGraphicsItem::ItemIsMovable,false);
                    agent->setFlag(QGraphicsItem::ItemIsSelectable,true); //TODO: show item details on selection (x,y,theta)?
                    it->second.shape=agent;
                    it->second.created=true;
                }
                double tmp=it->second.angle;
                while ( tmp>M_PI )
                    tmp=tmp-2*M_PI;
                it->second.shape->setPos(it->second.x,it->second.y);
                it->second.shape->setRotation(tmp*180/M_PI-90);
            }
        };
	virtual void paintBackground(QGraphicsScene */*painter*/){};
	virtual void timerEvent(std::shared_ptr<std::mutex>&,const world_sim_packet&){};
	virtual void init(){};
	virtual void keypress(){};
	virtual ~abstract_viewer_plugin()
	{
	};
	void* father;
	
protected:
	QPolygon agentshape;
	
};


#endif // ABSTRACT_VIEWER_PLUGIN_H
