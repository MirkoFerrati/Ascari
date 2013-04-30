 #include "monitor_viewer.h"

#include <mutex>
#include <memory>
 
 monitor_viewer::monitor_viewer(std::map< std::string, monitor_packet >* monitor_read, std::shared_ptr< std::mutex > monitor_read_mutex)
 {
      monitor=true; 
      this->monitor_read=monitor_read;
      this->monitor_read_mutex=monitor_read_mutex;

 }
// 
// void monitor_viewer::drawArrow(int x1,int y1, int x2, int y2, double sze, QPainter* painter )
// {
//     QPoint pd, pa, pb;
//     double tangent;
//     /* in order to get the arrowhead the correct way round, 
//      * we reverse the gradient. 
//      * This is equivalent to reversing the geometry below...
//      */
//     pd.setX(x2-x1);
//     pd.setY(y2 - y1);
//     
//     if (pd.x() == 0 && pd.y() == 0)
//         return;
//     tangent = atan2 ((double) pd.y(), (double) pd.x());
//     pa.setX(sze * cos (tangent + M_PI / 7) + x1);
//     pa.setY(sze * sin (tangent + M_PI / 7) + y1);
//     pb.setX(sze * cos (tangent - M_PI / 7) + x1);
//     pb.setY(sze * sin (tangent - M_PI / 7) + y1);
//     //-- connect the dots...
//     
//     painter->drawLine( pa.x(), pa.y(), x1, y1 );
//     painter->drawLine( pb.x(), pb.y(), x1, y1 );
//     
// }
// 
// void Viewer::paintEvent ( QPaintEvent */*event*/ )
// {
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
// 			    painter.scale(2,2);
//           painter.drawConvexPolygon(hourHand, 3);
// 				painter.save();
// 				painter.scale((scalingFactorX*3.0/sidex),(scalingFactorY*3.0/sidey));
// 				painter.scale(painter.fontMetrics().height()/70.0,-painter.fontMetrics().height()/70.0);
// 				painter.drawText(0,0,QString(it->first.substr(6).c_str()));
// 				painter.restore();
// 
//         
// 	
//       if (monitor )
//             {
//                 painter.save();
// 		painter.setBrush(Qt::NoBrush);
// 
//                 QPen pen;
//                 pen.setColor ( QColor ( "blue" ) );
//                 pen.setStyle ( Qt::DashLine );
//                 painter.setPen ( pen );
// 		//painter.drawEllipse(QPointF(it->second.x,it->second.y),RADIUS,RADIUS);
// 
// 		monitor_read_mutex->lock();
//                 if ( monitor_read->count ( it->first ) )
//                 {
//                 for ( auto target:monitor_read->at ( it->first ).agents )
//                     {
//                         if ( !target.ok )
// 			{
//                             painter.drawLine (it->second.x , it->second.y,agents.at ( target.agent_id ).x,agents.at ( target.agent_id ).y );
// 			     drawArrow (agents.at ( target.agent_id ).x,agents.at ( target.agent_id ).y,it->second.x , it->second.y,20,&painter );
// 			}
//                     }
//                 }
//                 monitor_read_mutex->unlock();
//                 painter.restore();
//             }
//               painter.restore();
// 
//     }
// }
// 
// void Viewer::keyPressEvent ( QKeyEvent *event )
// {
//     switch ( event->key() )
//     {
//     case Qt::Key_M:
//     {
//         monitor=!monitor;
//     }
//     }
// 
// }