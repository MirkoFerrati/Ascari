#ifndef MONITOR_VIEWER_H
#define MONITOR_VIEWER_H

#include <map>
#include<memory>
#include <types/monitor_packet.h>
#include "viewer_plugin.h"
#include <mutex>

class monitor_viewer:public viewer_plugin
{
  
public:
  monitor_viewer( std::map< std::string, monitor_packet >* monitor_read, std::shared_ptr< std::mutex > monitor_read_mutex );
  
  
private:  
      void drawArrow( int x1, int y1, int x2, int y2, double sze, QPainter* painter );
	std::map<std::string,monitor_packet> *monitor_read;
	std::shared_ptr<std::mutex > monitor_read_mutex;
	    bool monitor;

};

#endif // MONITOR_VIEWER_H
