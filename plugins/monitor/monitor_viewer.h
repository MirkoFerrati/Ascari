#ifndef MONITOR_VIEWER_H
#define MONITOR_VIEWER_H


#include <map>
#include<memory>
#include <types/monitor_packet.h>
#include <../simulator/visibility/map2d.h>
#include "../plugins/abstract_viewer_plugin.h"
#include <../communication/world_sniffer_abstract.h>
#include <mutex>

class monitor_viewer:public abstract_viewer_plugin
{
  
public:
  monitor_viewer( std::string mapfilename );
    monitor_viewer();
  
public:  
    void paintBackground ( QPainter& painter);
      void drawArrow( int x1, int y1, int x2, int y2, double sze, QPainter* painter );
      void paintAgents ( QPainter& painter,const std::map<std::string,Agent>& agents );
	bool monitor;
     void init ( std::string );
     ~monitor_viewer();
private:
         std::unique_ptr<world_sniffer_abstract> identifier_sniffer;
	 std::map<std::string,monitor_packet> monitor_read;
        std::shared_ptr<std::mutex> monitor_read_mutex; 
    std::string mapfilename;
    map2d* map;
};
#endif // MONITOR_VIEWER_H
