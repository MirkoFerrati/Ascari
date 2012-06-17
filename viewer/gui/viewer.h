#ifndef VIEWER
#define VIEWER


#include "Agent.h"
#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>
#include <boost/asio.hpp>
class Viewer : public QWidget
{

  public:
    Viewer(const std::vector<char>& buffer,boost::asio::io_service& io_service,QWidget *parent = 0);
    ~Viewer();
    void setScalingFactor(double scalingFactorX, double scalingFactorY);
    void setTranslateFactor(double tx=0,double ty=0);
    void setBackImage(std::string path);
    void start();
   
protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void pause();
   

  private:
    int timerId;
    int time;
    std::map<std::string,Agent> agents;
    std::vector<QColor> colors;
    double scalingFactorX, scalingFactorY, translateX, translateY;
    std::string backImage;
    QImage immagine;
    QPixmap pixmap;
    const std::vector<char>& buffer;
	    /// The size of a fixed length header.
    enum { header_length = 8 };
	world_sim_packet infos;
	boost::asio::io_service& io_service;
    void setScalingAndTranslateFactor(double maxX,double minX,double maxY,double minY);
	double maxX;
	double minX;
	double maxY;
	double minY;
	
   
};

#endif //VIEWER
