#include "formation_control_simulator.h"
#include "formation_control_parser.h"

#include <types/world_sim_packet.h>

formation_control_simulator::formation_control_simulator(simulator* s):sim_packet(s->sim_packet)
{
   
}


bool formation_control_simulator::initialize( const Parsed_World& w )
{
    return true;
}

void formation_control_simulator::stop()
{

}


void formation_control_simulator::run_plugin()
{

}
