#ifndef VIEWER
#define VIEWER


#include "Agent.h"
#include <lemon/smart_graph.h>
#include <lemon/path.h>
#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>
#include <boost/asio.hpp>
#include <mutex>
class Viewer : public QWidget
{

  public:
    Viewer(const std::vector< char >& buffer, boost::asio::io_service& io_service, QWidget* parent = 0, int view_type=0, std::string graphName="");
    Viewer(const world_sim_packet& read, std::shared_ptr<std::mutex>& read_mutex, QWidget* parent=0, int view_type=0, std::string graphName="");
    
    ~Viewer();
    void setScalingFactor(double scalingFactorX, double scalingFactorY);
    void setTranslateFactor(double tx=0,double ty=0);
    void setBackImage(std::string path);
    void start();
    int view_type;

   
protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);
	void parse_graph(std::string graphName);
    void pause();
   void init(std::string graphName);

  private:
    int timerId;
    int time;
    std::map<std::string,Agent> agents;
    std::vector<QColor> colors;
    double scalingFactorX, scalingFactorY, translateX, translateY;
    std::string backImage;
    QImage immagine;
    QPixmap pixmap;
    //const std::vector<char>& buffer;
    	const world_sim_packet& infos;

    std::shared_ptr<std::mutex>& mutex;
    enum { header_length = 8 };
	//boost::asio::io_service& io_service;
    void setScalingAndTranslateFactor(double maxX,double minX,double maxY,double minY);
	double maxX;
	double minX;
	double maxY;
	double minY;
	lemon::SmartDigraph graph;
    lemon::SmartDigraph::Node source, target;  
    lemon::SmartDigraph::Node next;
    lemon::SmartDigraph::ArcMap<int> *length;
    lemon::SmartDigraph::NodeMap<int> *coord_x, *coord_y;
   
};

#endif //VIEWER
