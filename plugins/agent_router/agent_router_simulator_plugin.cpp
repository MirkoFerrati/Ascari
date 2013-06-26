#include "agent_router_simulator_plugin.h"
#include <../gml2lgf/graph.h>
#include "../simulator/simulator.h"

void agent_router_simulator_plugin::stop()
{
  if (!graph_router)
  {
        graph_router->set_run ( false );
    graph_router->join_thread();
  }
}

bool agent_router_simulator_plugin::initialize ( const Parsed_World& /*w*/ )
{
  graph_router=new Udp_agent_router<graph_packet> (SIMULATOR_GRAPH_PORT,AGENT_GRAPH_PORT );
    graph_router->start_thread();
    
return true;
}

agent_router_simulator_plugin::agent_router_simulator_plugin(simulator* )
{
graph_router=0;
}


agent_router_simulator_plugin::~agent_router_simulator_plugin()
{
if (!graph_router)
{
          graph_router->set_run ( false );
    graph_router->join_thread();
}
delete graph_router;
graph_router=0;
}


void agent_router_simulator_plugin::run_plugin()
{
if (!graph_router)
{
  ERR("graph_router can not run if the thread was not created, maybe you didn't call initialize?",NULL);
  throw "not initialized";
}
}

