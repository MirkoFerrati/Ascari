#ifndef DEFINE_H
#define DEFINE_H
#include <string>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <fstream>
#include <logog.hpp>
class configuration_reader{
public:
  configuration_reader()
  {  
    boost::program_options::options_description desc;
    
    std::ifstream stream("ascari.cfg");
    if (!stream.is_open())
    {
	ERR("impossibile aprire il file ascari.cfg",NULL);
    }
    boost::program_options::store(boost::program_options::parse_config_file(stream,desc,true),vm);
    stream.close();
  };
  std::string getValue(std::string value)
  {
    return vm.at(value).as<std::string>();
  };
  std::string getValue(const char* value)
  {
    return vm.at(value).as<std::string>();
  };
private:
  boost::program_options::variables_map vm;
};

static configuration_reader CONFIG;



#define MULTICAST_PORT  30000
#define MULTICAST_ADDRESS "239.255.0.1"
#define SOCKET_BINDING "0.0.0.0"
#define MAX_PACKET_LENGTH 1024*32
#define SIMULATOR_PORT 10000
#define SIMULATOR_ROUTE_PORT 10050
#define AGENT_ROUTE_PORT 10051
#define SIMULATOR_GRAPH_PORT 10052
#define AGENT_GRAPH_PORT 10053


#define SYNC_PROTOCOL "tcp://localhost:5761"




#define T_CAMP 0.01

#endif //DEFINE_H
