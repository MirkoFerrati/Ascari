#ifndef DEFINE_H
#define DEFINE_H
#include <string>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <logog.hpp>
class configuration_reader{
public:
  configuration_reader()
  {  

   boost::property_tree::read_info("ascari.cfg",map);

  };
  std::string getValue(std::string value)
  {
    return map.get<std::string>(value);
  };
  std::string getValue(const char* value)
  {
    return map.get<std::string>(value);
  };
  bool exists(std::string value)
  {
    return map.count(value);
  };
  bool exists(const char* value)
  {
    return map.count(value);
  };
    const boost::property_tree::ptree& getMap()
    {
        return map;
    }

private:
    boost::property_tree::ptree map;
};

static configuration_reader CONFIG;



// #define MULTICAST_PORT  30000
// #define MULTICAST_ADDRESS "239.255.0.1"
// #define SOCKET_BINDING "0.0.0.0"
// #define SIMULATOR_PORT 10000
// #define SIMULATOR_ROUTE_PORT 10050
// #define AGENT_ROUTE_PORT 10051
// #define SIMULATOR_GRAPH_PORT 10052
// #define AGENT_GRAPH_PORT 10053
//#define SYNC_PROTOCOL "tcp://localhost:5761"



#define MULTICAST_PORT  atoi(CONFIG.getValue("MULTICAST_PORT").c_str())
#define MULTICAST_ADDRESS CONFIG.getValue("MULTICAST_ADDRESS")
#define SOCKET_BINDING CONFIG.getValue("SOCKET_BINDING")
#define SIMULATOR_PORT atoi(CONFIG.getValue("SIMULATOR_PORT").c_str())
#define SIMULATOR_ROUTE_PORT atoi(CONFIG.getValue("SIMULATOR_ROUTE_PORT").c_str())
#define AGENT_ROUTE_PORT atoi(CONFIG.getValue("AGENT_ROUTE_PORT").c_str())
#define SIMULATOR_GRAPH_PORT atoi(CONFIG.getValue("SIMULATOR_GRAPH_PORT").c_str())
#define AGENT_GRAPH_PORT atoi(CONFIG.getValue("AGENT_GRAPH_PORT").c_str())
#define SYNC_PROTOCOL CONFIG.getValue("DEFAULT_SYNC_PROTOCOL").c_str()


#define MAX_PACKET_LENGTH 1024*32

#endif //DEFINE_H
