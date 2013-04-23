// #include "task_assignment_viewer.h"
// 
//     started=false;
// 
// 
// void Viewer::timerEvent ( QTimerEvent */*event*/ )
// {
//     mutex->lock();
//     //written by Alessandro Settimi
//     if (view_type==4)
//     {
// 	for(auto i=infos.objects.begin(); i!=infos.objects.end(); ++i)
// 	{
// 	    tasks[i->first]=*(reinterpret_cast<const task_assignment_namespace::task*>(i->second.getState()));
// 	    setScalingAndTranslateFactor(tasks[i->first].task_position[0],tasks[i->first].task_position[0]-2,tasks[i->first].task_position[1],tasks[i->first].task_position[1]-2);
// 	}
//     }
//     //written by Alessandro Settimi
//     mutex->unlock();
// 
// }
// 
// 
// void Viewer::paintEvent ( QPaintEvent */*event*/ )
// {
//   
//       //written by Alessandro Settimi
//     if (view_type==4)
//     { 
//       
// 	std::stringstream app(std::stringstream::out);
// 	app.str("");
// 	
// 	for (auto i=tasks.begin(); i!=tasks.end(); ++i)
// 	{
// 		task_assignment_namespace::task task=i->second;
// 		
// 		painter.save();
// 		painter.translate(task.task_position[0],task.task_position[1]);
// 
// 
// 		if (task.task_type == 0)
// 		{
// 			painter.setBrush(QColor("lightgreen"));
// 		}	
// 		else 			
// 		{
// 			if(task.available)
// 			{
// 				
// 				painter.setBrush(QColor("cyan"));
// 			}
// 			else
// 			{
// 				painter.setBrush(QColor("white"));
// 			}
// 		}
// 
// 		painter.drawRect(-1,-1,2,2);
// 		painter.restore();
// 
// 
// 		if (task.executing)
// 		{
// 			app <<  task.id.c_str() << " (" << ((task.task_execution_time-time+floor(task.time))>0?(task.task_execution_time-time+floor(task.time)):0) << ")";
// 		}
// 		else
// 		{
// 			app << task.id.c_str() << " (" << task.task_execution_time << ")";
// 		}
// 
// 		painter.save();
// 		painter.translate(task.task_position[0],task.task_position[1]);
// 		painter.setBrush(QColor("black"));
// 		painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.0);
// 		painter.drawText(0,75,QString(app.str().c_str()));
// 		painter.restore();
// 
// 		app.str("");
// 
// 		if (task.task_deadline != 0)
// 		{
// 			app << "[" << (((task.task_deadline-time)>=0)?(task.task_deadline-time):(0)) << "]";
// 		  
// 			painter.save();
// 			painter.translate(task.task_position[0],task.task_position[1]);
// 			painter.setPen(QColor("red"));
// 			painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.0);
// 			painter.drawText(0,-65,QString(app.str().c_str()));
// 			painter.restore();
// 			
// 			app.str("");
// 		}
// 	}
//     }
//     
//     if(!agents.empty() && (!started || old_time>time))
//     {
// 	      for ( map<string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
// 	      {
// 			std::vector<double> app;
// 	      
// 			app.push_back(it->second.x);
// 			app.push_back(it->second.y);
// 			
// 			initial_pos[it->first]=app;
// 	      }
// 	      
// 	      started=true;
//     }
//     
//     old_time=time;
//      //written by Alessandro Settimi 
//   
//   for ( map<string,Agent>::const_iterator it=agents.begin(); it!=agents.end(); ++it )
//     {
//       
//           painter.save();
//         painter.setBrush ( QColor ( "red" ) );
//         painter.translate ( it->second.x,it->second.y );
//         //TODO: Pessimo: lo zero degli angoli parte dall'asse y invece che da x
//         double tmp=it->second.angle;
//         while ( tmp>M_PI )
//             tmp=tmp-2*M_PI;
//         painter.rotate ( ( tmp*180/M_PI )-90 );
// 
//         
// 			    painter.scale(0.2,0.2);
//             painter.drawConvexPolygon(hourHand, 3);
// 			
// 
// 				painter.save();
// 				painter.scale((scalingFactorX*3.0/sidex),(scalingFactorY*3.0/sidey));
// 				painter.scale(painter.fontMetrics().height()/10.0,-painter.fontMetrics().height()/10.0);
// 				painter.drawText(0,0,QString(it->first.substr(6).c_str()));
// 				painter.restore();
// 
//         
// 	
//       if (time>0)
// 	{ 
// 		painter.save();
// 		painter.translate(initial_pos.at(it->first).at(0),initial_pos.at(it->first).at(1));
// 		painter.drawArc(-1,-1,2,2,0,16*360);
// 		painter.restore();
// 		
// 		std::stringstream tm(std::stringstream::out);
// 		tm << (int)it->second.charge << "%";
// 		
// 		painter.save();
// 		painter.translate(it->second.x,it->second.y);
// 		painter.setPen(QColor("green"));
// 		painter.scale(painter.fontMetrics().height()/1000.0,-painter.fontMetrics().height()/1000.0);
// 		painter.drawText(-20,-20,QString(tm.str().c_str()));
// 		painter.restore();
// 		
// 		//per la scia
// 
// 		positions[it->first].set_capacity(600);
// 		positions[it->first].push_back(it->second.x);
// 		positions[it->first].push_back(it->second.y);
// 		
// 		for (unsigned int i=0;i<positions[it->first].size();i=i+2)
// 		{
// 			painter.save();
// 			painter.translate(positions[it->first].at(i),positions[it->first].at(i+1));
// 			painter.setPen(QColor("black"));
// 			painter.drawPoint(0,0);
// 			painter.restore();
// 		}
// 	}
// 	        painter.restore();
// 
//     }
// }
// 
