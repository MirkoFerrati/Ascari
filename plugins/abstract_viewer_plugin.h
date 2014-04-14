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
#include <qt4/QtGui/qpolygon.h>

#define TYPE_AGENT 1234
#define TYPE_TEXT 5678

class abstract_viewer_plugin
{
public:
	
	abstract_viewer_plugin()
	{
            agentshape=QPolygon(QVector<QPoint>( { QPoint ( -10, 10 ),QPoint ( -10, -10 ),QPoint ( 30, 0 )}));
            
	}
	void setfather ( void* father )
	{
		assert ( father );
		this->father=father;
	};
	
	virtual void paintAgents(QGraphicsScene* Scene, std::map<std::string,Agent>& agents)
	{
            if (agents.empty())
                WARN("%s","No agents to paint\n");
            for ( std::map<std::string,Agent>::iterator it=agents.begin(); it!=agents.end(); ++it )
            {
                if (!it->second.created)
                {
                    QGraphicsTextItem *text=Scene->addText(QString::fromStdString(it->first));
                    text->scale(1,-1);
                    text->setData(0,TYPE_TEXT);
                    text->setFlag(QGraphicsItem::ItemIsMovable,false);
                    text->setFlag(QGraphicsItem::ItemIsSelectable,true); //TODO: show item details on selection (x,y,theta)?
                    
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
                    it->second.text=text;
                    it->second.created=true;
                }
                double tmp=it->second.angle;
                while ( tmp>M_PI )
                    tmp=tmp-2*M_PI;
                it->second.shape->setPos(it->second.x,it->second.y);
                it->second.text->setPos(it->second.x,it->second.y);
                
                it->second.shape->setRotation(tmp*180.0/M_PI);
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
