#ifndef MONITOR_VIEWER_H
#define MONITOR_VIEWER_H

#include <map>
#include<memory>

class monitor_viewer
{
  
public:
  monitor_viewer();
  
private:  
      void drawArrow( int x1, int y1, int x2, int y2, double sze, QPainter* painter );
	std::map<std::string,monitor_packet> *monitor_read;
	std::shared_ptr<std::mutex > monitor_read_mutex;
	    bool monitor;

};

#endif // MONITOR_VIEWER_H
