#ifndef VIEWER
#define VIEWER

#include "AgentPath.h"
#include "Agent.h"
#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>
#include "wall.h"
#include "TargetList.h"
class Viewer : public QWidget
{
  Q_OBJECT

  public:
    Viewer(QWidget *parent = 0);
    ~Viewer();
    void setAgents(vector<Agent> a);
    void setWallState(vector<wallState> w);
    void setEndTime(int time);
    void setScalingFactor(int scalingFactorX=1,int scalingFactorY=1);
    void setTranslateFactor(int tx=0,int ty=0);
    void startGame();
    void setBackImage(string path);
    void setTargets(vector<TargetList> t);
protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void pauseGame();
    void stopGame();
    void victory();
    void checkCollision();


  private:
    void paintWall(QPainter* painter);
    int x;
    int timerId;
    int time;
    //vector<AgentPath> paths;
    vector<Agent> agents;
    vector<QColor> colors;
    int endtime;
    bool gameOver;
    bool gameWon;
    bool gameStarted;
    bool paused;
    int scalingFactorX, scalingFactorY, translateX, translateY;
    string backImage;
    QImage immagine;
    QPixmap pixmap;
    vector<wallState> walls;
};

#endif //VIEWER
