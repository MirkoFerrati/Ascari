//written by Alessandro Settimi
#include "task_assignment_viewer.h"

#include "../HACK_KDEVELOP.h"
#include <qt4/QtGui/QGraphicsScene>
#include <qt4/QtGui/QGraphicsRectItem>
#include <qt4/QtGui/QGraphicsTextItem>
#ifdef ISVIEWER

task_assignment_viewer::task_assignment_viewer( std::shared_ptr<std::mutex>& mutex,const world_sim_packet& infos)//:time(infos.time),mutex(mutex),infos(infos)
{
    started=false;
    old_time=0;
    size=0.0;
}
task_assignment_viewer::task_assignment_viewer()
{
    started=false;
    old_time=0;
    size=0.0;
}


void task_assignment_viewer::timerEvent ( std::shared_ptr< std::mutex >& mutex, const world_sim_packet& infos )
{
    Viewer* temp_father=reinterpret_cast<Viewer*>(father);
    mutex->lock();
    old_time=now_time;
    now_time=infos.time;

    if (infos.object_list.objects.count("TASK_ASSIGNMENT"))
    {
        for(auto i:infos.object_list.objects.at("TASK_ASSIGNMENT"))
        {
            //TODO: controllare che SIA UN TASK
            //TODO: far funzionare queste righe
            tasks[i->name]=(reinterpret_cast<task_assignment_task*>(i))->state;
            
/*          int count=1;

            if(diff1 < diff2) diff1=diff2;

            while(diff1>10) {
                diff1/=10.0;
                count++;
            }

            size=diff1*count/10.0;

            setAgentSize(size);*/
        }
    }

    mutex->unlock();
}


void task_assignment_viewer::paintBackground (QGraphicsScene* scene)
{
    
    std::stringstream app(std::stringstream::out);
    app.str("");

    for (auto i=tasks.begin(); i!=tasks.end(); ++i)
    {
        task_assignment_namespace::task& task=i->second;

        if (!viewer_tasks.count(i->first))
        {
            auto temp=scene->addRect(0,0,20,20);
            viewer_tasks[i->first]=temp;
            viewer_text_tasks[i->first]=scene->addText("");
            viewer_deadline_tasks[i->first]=scene->addText("");
            viewer_deadline_tasks[i->first]->setDefaultTextColor(QColor("red"));
        }
        
        viewer_tasks[i->first]->setPos(task.task_position[0],task.task_position[1]);

        if (task.task_type == 0)
        {
            viewer_tasks[i->first]->setBrush(QColor("lightgreen"));           
        }
        else
        {
            if(task.available)
            {

                viewer_tasks[i->first]->setBrush(QColor("cyan"));
            }
            else
            {
                viewer_tasks[i->first]->setBrush(QColor("white"));
            }
        }

        if (task.executing)
        {
            app <<  task.id.c_str() << " (" << ((task.task_execution_time-now_time+floor(task.time))>0?(task.task_execution_time-now_time+floor(task.time)):0) << ")";
        }
        else
        {
            app << task.id.c_str() << " (" << task.task_execution_time << ")";
        }
// 		std::cout<<app.str().c_str()<<std::endl;

        viewer_text_tasks[i->first]->setPos(task.task_position[0],task.task_position[1]);
        viewer_text_tasks[i->first]->setPlainText(QString(app.str().c_str()));
        
        app.str("");

        if (task.task_deadline != 0)
        {
            app << "[" << (((task.task_deadline-now_time)>=0)?(task.task_deadline-now_time):(0)) << "]";
        }
        viewer_deadline_tasks[i->first]->setPos(task.task_position[0],task.task_position[1]);
        viewer_deadline_tasks[i->first]->setPlainText(QString(app.str().c_str()));
        app.str("");
        
    }
    
}

void task_assignment_viewer::paintAgents(QGraphicsScene* scene, std::map< std::string, Agent >& agents)
{
    abstract_viewer_plugin::paintAgents(scene,agents);
    for ( std::map<std::string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
    {
        if (!my_agents.count(it->first))
        {
            auto temp=scene->addEllipse(-size*2,-size*2,4*size,4*size);
            temp->setPos(it->second.x,it->second.y);
            my_agents[it->first].initial_pos=temp;
            my_agents[it->first].charge=scene->addText("");
            my_agents[it->first].charge->setDefaultTextColor(QColor("green"));
            my_agents[it->first].positions.set_capacity(60);
            for (int i=0;i<60;i++)
                my_agents[it->first].positions.push_back(scene->addRect(0,0,1,1));
            my_agents[it->first].it=my_agents[it->first].positions.begin();
        }
    }

    if(now_time<10)
    {
        for ( std::map<std::string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
        {
            my_agents[it->first].initial_pos->setPos(it->second.x,it->second.y);
        }
    }

    old_time=now_time;

    for ( std::map<std::string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
    {
        if (now_time>0)
        {
            std::stringstream tm(std::stringstream::out);
            tm << (int)it->second.charge << "%";
            my_agents.at(it->first).charge->setPos(it->second.x,it->second.y);
            //per la scia

            (*my_agents.at(it->first).it)->setPos(it->second.x,it->second.y);
            ++it;
        }
    }
}

task_assignment_viewer::~task_assignment_viewer()
{

}

#endif
