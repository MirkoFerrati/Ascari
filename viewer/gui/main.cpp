#include <QtGui/QApplication>
#include "viewer.h"
#include "filereader.h"
#include <QtGui/QDesktopWidget>

#include <parser.h>



void center(QWidget &widget,int WIDTH=800,int HEIGHT=800)
{
  int x, y;
  int screenWidth;
  int screenHeight;



  QDesktopWidget *desktop = QApplication::desktop();

  screenWidth = desktop->width();
  screenHeight = desktop->height();

  x = (screenWidth - WIDTH) / 2;
  y = (screenHeight - HEIGHT) / 2;

  widget.setGeometry(x, y, WIDTH, HEIGHT);
  //widget.setFixedSize(WIDTH, HEIGHT);
}


int main(int argc, char *argv[])
{
  //FileReader reader(argc, argv);
    Parser parser(argc,argv);
    //cout<<parser.toString();
    QApplication app(argc,argv);

    Viewer window;
    window.setScalingFactor(parser.getMaxX()-parser.getMinX(),parser.getMaxY()-parser.getMinY());
    window.setTranslateFactor((parser.getMaxX()+parser.getMinX())/2,(parser.getMaxY()+parser.getMinY())/2);

    vector<Agent> agents;
    set<string> agents_name=parser.getAgentsName();
    for (set<string>::iterator it=agents_name.begin();it!=agents_name.end();it++)
    {
        Agent a((*it),parser.getPaths(*it),parser.getTargets(*it));
        agents.push_back(a);
    }

    window.setAgents(agents);
    window.setEndTime(parser.getTime());
    window.setWallState(parser.getWalls());
    window.setBackImage(parser.getImagePath());
  window.setWindowTitle("Breakout");
  window.show();
  center(window);
  window.startGame();

  return app.exec();
}
