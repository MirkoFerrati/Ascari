#include "nostop_simulator_plugin.h"
#include "nostop_parser_plugin.h"
//#include <objects/task_assignment_task.h>
#include <types/world_sim_packet.h>
#include "nostop_discretized_area.h"
#include "nostop_packet.hpp"

namespace NoStop 
{

	///
	Simulator_plugin::Simulator_plugin(simulator* s)
		:	sim_packet(s->sim_packet), m_router(nullptr), m_router_started(false), m_area(nullptr)
	{}

	///
	bool Simulator_plugin::initialize ( const Parsed_World& w )
	{

		auto world=reinterpret_cast<NoStop::Parsed_world*>(w.parsed_items_from_plugins[0]);
		//this->world=*world;

		//task_assignment_algorithm = world->task_assignment_algorithm;
		m_num_agents=w.agents.size();

		return true;
	}

	///
	void Simulator_plugin::stop()
	{
		//TODO
	}

	///
	void Simulator_plugin::run_plugin()
	{
		if ( !m_router_started )
		{
			m_router = std::make_shared< Router<Coverage_packet> > ( m_num_agents );
			m_router_started=true;
		}
	}
}
