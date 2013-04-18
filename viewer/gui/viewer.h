#ifndef VIEWER
#define VIEWER

//written by Alessandro Settimi
#include "yaml_parser.h"
#include <typedefs.h>
#include <boost/circular_buffer.hpp>
//written by Alessandro Settimi

#include "Agent.h"
#include <lemon/smart_graph.h>
#include <lemon/path.h>
#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>
#include <boost/asio.hpp>
#include <mutex>
#include <map>
class Viewer : public QWidget
{

  public:
    //Viewer(const std::vector< char >& buffer, boost::asio::io_service& io_service, QWidget* parent = 0, int view_type=0, std::string graphName="");
    Viewer(const world_sim_packet& read, std::shared_ptr<std::mutex>& read_mutex, QWidget* parent=0, int view_type=0, std::string graphName="");
    
    ~Viewer();
    void setScalingFactor(double scalingFactorX, double scalingFactorY);
    void setTranslateFactor(double tx=0,double ty=0);
    void setBackImage(std::string path);
    void start();
    void setMonitor(std::map<std::string,monitor_packet>* monitor_read, std::shared_ptr< std::mutex > monitor_read_mutex);
    int view_type;

    //written by Alessandro Settimi
    void initialize_tasks(const std::map<std::string,task_assignment_task>& obj_tasks);
    //written by Alessandro Settimi

   
protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);
	void parse_graph(std::string graphName);
    void pause();
   void init(std::string graphName);
void closeEvent(QCloseEvent *event);
  private:
    int timerId;
    int time;
    bool monitor;
    std::map<std::string,Agent> agents;
    std::vector<QColor> colors;
    double scalingFactorX, scalingFactorY, translateX, translateY;
    std::string backImage;
    QImage immagine;
    QPixmap pixmap;
    //const std::vector<char>& buffer;
    const world_sim_packet& infos;
    std::shared_ptr<std::mutex> mutex;
	std::map<std::string,monitor_packet> *monitor_read;
	std::shared_ptr<std::mutex > monitor_read_mutex;
	
	enum { header_length = 8 };
	//boost::asio::io_service& io_service;
    void setScalingAndTranslateFactor(double maxX,double minX,double maxY,double minY);
    void drawArrow( int x1, int y1, int x2, int y2, double sze, QPainter* painter );
	double maxX;
	double minX;
	double maxY;
	double minY;
	lemon::SmartDigraph graph;
    lemon::SmartDigraph::Node source, target;  
    lemon::SmartDigraph::Node next;
    lemon::SmartDigraph::ArcMap<int> *length;
    lemon::SmartDigraph::NodeMap<int> *coord_x, *coord_y;
    
    //written by Alessandro Settimi    
    std::map<task_assignment_namespace::agent_id,std::vector<double>> initial_pos;
    bool started;
    double old_time;
    std::map<task_assignment_namespace::agent_id,boost::circular_buffer<double>> positions;
    std::map<task_assignment_namespace::task_id,task_assignment_namespace::task> tasks;
    //written by Alessandro Settimi
   
};

#endif //VIEWER
