#include "viewer.h"
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <math.h>
#include <QtCore/QLocale>
#include <QtGui/QWidget>
Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
{
  time=0;
  backImage="";
  paused = FALSE;
  gameStarted = FALSE;
}

void Viewer::setScalingFactor(int scalingFactorX,int scalingFactorY)
{
    this->scalingFactorX=scalingFactorX;
    this->scalingFactorY=scalingFactorY;
}

void Viewer::setTranslateFactor(int translateX, int translateY)
{
    this->translateX=translateX;
    this->translateY=translateY;
}

void Viewer::setWallState(vector<wallState> w)
{
    walls=w;
}

void Viewer::setAgents(vector<Agent> a)
{
   agents.swap(a);
   for (int i=0;i<agents.size();i++)
   {
       colors.push_back(QColor::fromHsv(1.0*i*359.0/agents.size(),180,180));
   }
}

void Viewer::setBackImage(string path){
    this->backImage=path;
    if (backImage.compare("")){
        immagine=QImage(QString(backImage.c_str()));
        immagine=immagine.scaled(QSize(width(),height()),
                                       Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    }
    pixmap.convertFromImage(immagine);
}

void Viewer::setEndTime(int time)
{
    endtime=time;
}

Viewer::~Viewer() {

}

void Viewer::paintEvent(QPaintEvent *event)
{
    static const QPoint hourHand[3] = {
        QPoint(3, -3),
        QPoint(-3, -3),
        QPoint(0, 4)
    };
    QColor hourColor(127, 0, 127);

    double sidex=width();
    double sidey=height();
    QPainter painter(this);

    if (backImage.compare("")){
        //QImage immagine=QImage(QString(backImage.c_str()));
        //painter.drawImage(QRect(0,0,sidex,sidey),immagine);
        painter.drawPixmap(0,0,sidex,sidey,pixmap);
    }

    if (true||gameStarted)
    {
    painter.save();
    QFont f = painter.font();
    f.setPointSizeF(height()/25.0);
    painter.setFont(f);
    painter.setPen(QColor("blue"));
    QString s;
    s.setNum(time);
    painter.drawText(width()/2,1.1*painter.fontMetrics().height(),s);
    painter.restore();
    }
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(hourColor);
    painter.translate(sidex/2,sidey/2);

    painter.scale(sidex/scalingFactorX,-sidey/scalingFactorY);

    painter.translate(-translateX,-translateY);


 if(gameStarted) {
     QPen pen;
     pen.setCapStyle(Qt::RoundCap);
     QVector<qreal> dashes;
     qreal space = 5;
     dashes << 1 << space << 1 << space << 1 << space
                << 1 << space << 1 << space;
     pen.setDashPattern(dashes);
     pen.setWidth(1);

     paintWall(&painter);
      for (unsigned int i=0;i<agents.size();i++)
      {
        painter.save();
        pen.setDashPattern(dashes);
        pen.setDashOffset(i);
        pen.setColor(colors[i]);
        painter.setPen(pen);
        int tsize=agents[i].targetlist.x.size();
          for (int j=0;j<tsize-2;j++)
          {
              painter.drawLine(agents[i].targetlist.x[j],agents[i].targetlist.y[j],
                               agents[i].targetlist.x[j+1],agents[i].targetlist.y[j+1]);
          }

          if (tsize>0)
          {
              pen.setStyle(Qt::SolidLine);
              painter.setPen(pen);
              painter.drawLine(agents[i].targetlist.x[tsize-1]-3,
                               agents[i].targetlist.y[tsize-1]-3,
                               agents[i].targetlist.x[tsize-1]+3,
                               agents[i].targetlist.y[tsize-1]+3
                               );
              painter.drawLine(agents[i].targetlist.x[tsize-1]-3,
                               agents[i].targetlist.y[tsize-1]+3,
                               agents[i].targetlist.x[tsize-1]+3,
                               agents[i].targetlist.y[tsize-1]-3
                               );

          }
        painter.restore();
        painter.save();
        painter.setBrush(colors[i]);
        painter.translate(agents[i].x,agents[i].y);
        //TODO: Pessimo: lo zero degli angoli parte dall'asse y invece che da x
        painter.rotate(agents[i].angle*180/M_PI-90);
        painter.scale((scalingFactorX*2.0/sidex),(scalingFactorY*2.0/sidey));
        painter.drawConvexPolygon(hourHand, 3);
        painter.restore();
    }
  }
}

void Viewer::paintWall(QPainter* painter)
{
    for (unsigned int i=0;i<walls.size();i++)
    {
        painter->save();
        painter->setPen(QColor("blue"));
        painter->drawRect(walls[i].getRect());
        double factor= 1.0* walls[i].getRect().width() / painter->fontMetrics().width(QString(walls[i].name.c_str()));
        QFont f = painter->font();
        f.setPointSizeF(f.pointSizeF()*factor*0.9);
        painter->setFont(f);
        painter->setPen(QColor("orangered"));
        painter->translate(walls[i].getRect().bottomLeft().rx(),walls[i].getRect().bottomLeft().ry()-painter->fontMetrics().height());
        painter->scale(1,-1);
        painter->drawText(0,0,QString(walls[i].name.c_str()));
        painter->restore();
    }
}

void Viewer::timerEvent(QTimerEvent *event)
{
    for (unsigned int i=0;i<agents.size();i++)
    {
    agents[i].translate(time);
    }
  repaint();
  time++;
  if (time>endtime)
      killTimer(timerId);
}



void Viewer::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {

    case Qt::Key_P:
        {
          pauseGame();
        }
        break;
    case Qt::Key_R:
    {
        time=0;
        killTimer(timerId);
        timerId = startTimer(10);
        paused=false;
    }
    break;
    case Qt::Key_Space:
        {
          startGame();
        }
        break;
    case Qt::Key_Escape:
        {
          qApp->exit();
        }
        break;
    case Qt::Key_Q:
        {
          qApp->exit();
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void Viewer::startGame()
{
  if (!gameStarted) {
    gameStarted = TRUE;
    timerId = startTimer(10);
  }
}

void Viewer::pauseGame()
{
  if (paused) {
    timerId = startTimer(10);
    paused = FALSE;
  } else {
    paused = TRUE;
    killTimer(timerId);
   }
}

