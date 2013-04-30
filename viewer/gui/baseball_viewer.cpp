#include "baseball_viewer.h"

// void Viewer::paintEvent ( QPaintEvent */*event*/ )
// {
// 
//         painter.save();
//         painter.setBrush ( QColor ( "lightgreen" ) );
//         painter.drawRect ( 0,0,sidex,sidey );
//         painter.restore();
// 
// 
//         painter.save();
//         painter.setBrush ( hourColor );
//         painter.translate ( sidex/2,sidey/2 );
//         painter.scale ( sidex/scalingFactorX,-sidey/scalingFactorY );
//         painter.setBrush ( QColor ( "green" ) );
//         painter.translate ( -translateX,-translateY );
//         painter.rotate ( 45 );
//         painter.drawRect ( 0,0,20,20 );
//         painter.restore();
// 
//         painter.save();
//         painter.translate ( 0,0 );
//         painter.rotate ( 45 );
//         painter.setBrush ( QColor ( "white" ) );
//         painter.drawRect ( -1,-1,2,2 );
//         painter.restore();
// 
//         painter.save();
//         painter.setBrush ( QColor ( "red" ) );
//         painter.translate ( 0,5 );
//         painter.rotate ( 180 );
//         painter.scale ( ( scalingFactorX*3.0/sidex ), ( scalingFactorY*3.0/sidey ) );
//         painter.drawConvexPolygon ( hourHand,3 );
//         painter.restore();
// 
//         painter.save();
//         painter.setBrush ( QColor ( "white" ) );
//         painter.translate ( 10,10 );
//         painter.rotate ( 45 );
//         painter.scale ( ( scalingFactorX*4.0/sidex ), ( scalingFactorY*4.0/sidey ) );
//         painter.drawRect ( -2,-2,5,5 );
//         painter.restore();
// 
//         painter.save();
//         painter.setBrush ( QColor ( "white" ) );
//         painter.translate ( -10,10 );
//         painter.rotate ( 45 );
//         painter.scale ( ( scalingFactorX*4.0/sidex ), ( scalingFactorY*4.0/sidey ) );
//         painter.drawRect ( -2,-2,5,5 );
//         painter.restore();
// 
//         painter.save();
//         painter.setBrush ( QColor ( "white" ) );
//         painter.translate ( 0,20 );
//         painter.rotate ( 45 );
//         painter.scale ( ( scalingFactorX*4.0/sidex ), ( scalingFactorY*4.0/sidey ) );
//         painter.drawRect ( -2,-2,5,5 );
//         painter.restore();
//     
//     
//  
//   
// }