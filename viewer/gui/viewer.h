#ifndef VIEWER
#define VIEWER

#include "Agent.h"
#include "viewer_plugin.h"
#include <types/world_sim_packet.h>
#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>
#include <mutex>
#include <map>
class Viewer : public QWidget
{

public:
    Viewer ( const world_sim_packet& read, std::shared_ptr< std::mutex >& read_mutex, QWidget* parent = 0 );

    ~Viewer();
    void setScalingFactor ( double scalingFactorX, double scalingFactorY );
    void setTranslateFactor ( double tx=0,double ty=0 );
    void setBackImage ( std::string path );
    void start();
    void init ( std::string filename );

void addPlugin(viewer_plugin* plugin);

const int * getTime(){return &simulation_time;}

private:

    void paintEvent ( QPaintEvent *event );
    void timerEvent ( QTimerEvent *event );
    void keyPressEvent ( QKeyEvent *event );
    void pause();
    void closeEvent ( QCloseEvent *event );
    void setScalingAndTranslateFactor ( double maxX,double minX,double maxY,double minY );
    std::vector<viewer_plugin*> plugins;
    int timerId;
    int simulation_time;
    std::map<std::string,Agent> agents;
    std::string backImage;
    QImage immagine;
    QPixmap pixmap;
    //const std::vector<char>& buffer;
    const world_sim_packet& infos;
    std::shared_ptr<std::mutex> mutex;
public:
    double scalingFactorX, scalingFactorY, translateX, translateY;

    double maxX;
    double minX;
    double maxY;
    double minY;

};

#endif //VIEWER
