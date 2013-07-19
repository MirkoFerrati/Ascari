#ifndef FAKE_SIMULATOR_H
#define FAKE_SIMULATOR_H
#include <boost/iterator/iterator_concepts.hpp>
#include <yaml_parser.h>

class fake_simulator
{
public:
  void create_communicator(int type,const Parsed_World& world);
  void initialize(const Parsed_World& world);
  void setPeriod(int period);
  void start_sim(int count=1000);
private:
    zmq_agent_communicator* communicator;
};

#endif // FAKE_SIMULATOR_H
