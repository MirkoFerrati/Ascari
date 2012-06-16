#ifndef WALL_H
#define WALL_H
#include <QtGui/QImage>
#include <QtCore/QRect>
#include "string"
using namespace std;

class wallState
{
public:
  double Wall_centerx;
  double Wall_centery;
  double Wall_width;
  double Wall_height;
  string name;
  QImage image;
  QRect rect;
  bool set;
public:
  wallState()
  {
    image.load("Rectangle.png");
    set=false;
  }

  QRect getRect()
  {
      if (set)
      {
        return rect;
      }
      else
      {
          set=true;
          rect.setHeight(Wall_height);
          rect.setWidth(Wall_width);
          rect.moveCenter(QPoint(Wall_centerx,Wall_centery));
          //rect.setRect(Wall_centerx,Wall_centery,Wall_width,Wall_height);
          return rect;
      }
  }

  QImage & getImage()
  {
    return image;
  }


};

#endif // WALL_H
