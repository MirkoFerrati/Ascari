//written by Alessandro Settimi
#include "task_assignment_viewer.h"

task_assignment_viewer::task_assignment_viewer( int* time ,std::shared_ptr<std::mutex>& mutex,const world_sim_packet& infos):time(time),mutex(mutex),infos(infos)
{
    started=false;
    old_time=0;
}


void task_assignment_viewer::timerEvent()
{
    Viewer* temp_father=reinterpret_cast<Viewer*>(father);
    mutex->lock();

    for(auto i=infos.objects.begin(); i!=infos.objects.end(); ++i)
    {
	tasks[i->first]=*(reinterpret_cast<const task_assignment_namespace::task*>(i->second.getState()));
	
	if ( temp_father->maxX< tasks[i->first].task_position[0] ) temp_father->maxX= tasks[i->first].task_position[0]*1.5;
	if ( temp_father->maxY< tasks[i->first].task_position[1] ) temp_father->maxY= tasks[i->first].task_position[1]*1.5;
	if ( temp_father->minX> tasks[i->first].task_position[0] ) temp_father->minX= tasks[i->first].task_position[0]*1.5;
	if ( temp_father->minY> tasks[i->first].task_position[1] ) temp_father->minY= tasks[i->first].task_position[1]*1.5;
    }
    
    mutex->unlock();
}


void task_assignment_viewer::paintBackground (QPainter& painter)
{  
	std::stringstream app(std::stringstream::out);
	app.str("");
	
	for (auto i=tasks.begin(); i!=tasks.end(); ++i)
	{ 
		task_assignment_namespace::task task=i->second;
		
		painter.save();
		painter.translate(task.task_position[0],task.task_position[1]);


		if (task.task_type == 0)
		{
			painter.setBrush(QColor("lightgreen"));
		}	
		else 			
		{
			if(task.available)
			{
				
				painter.setBrush(QColor("cyan"));
			}
			else
			{
				painter.setBrush(QColor("white"));
			}
		}

		painter.drawRect(-1,-1,2,2);
		painter.restore();


		if (task.executing)
		{
			app <<  task.id.c_str() << " (" << ((task.task_execution_time-*time+floor(task.time))>0?(task.task_execution_time-*time+floor(task.time)):0) << ")";
		}
		else
		{
			app << task.id.c_str() << " (" << task.task_execution_time << ")";
		}
// 		std::cout<<app.str().c_str()<<std::endl;
		
		painter.save();
		painter.translate(task.task_position[0],task.task_position[1]);
		painter.setBrush(QColor("black"));
 		painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.00);
		painter.drawText(0,75,QString(app.str().c_str()));
		painter.restore();

		app.str("");

		if (task.task_deadline != 0)
		{
			app << "[" << (((task.task_deadline-*time)>=0)?(task.task_deadline-*time):(0)) << "]";
		  
			painter.save();
			painter.translate(task.task_position[0],task.task_position[1]);
			painter.setPen(QColor("red"));
			painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.00);
			painter.drawText(0,-65,QString(app.str().c_str()));
			painter.restore();
			
			app.str("");
		}
	}
}

void task_assignment_viewer::paintAgents(QPainter& painter,const std::map<std::string,Agent>& agents)
{
	abstract_viewer_plugin::paintAgents(painter,agents);
	
	for ( std::map<std::string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
	{
    
		if(!agents.empty() && (!started || old_time>*time))
		{
			  for ( std::map<std::string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
			  {
				    std::vector<double> app;
			  
				    app.push_back(it->second.x);
				    app.push_back(it->second.y);
				    
				    initial_pos[it->first]=app;
			  }
			  
			  started=true;
		}
	    
		old_time=*time;
		
		
		if (*time>0)
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
			painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.00);
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
	}
}

task_assignment_viewer::~task_assignment_viewer()
{

}

