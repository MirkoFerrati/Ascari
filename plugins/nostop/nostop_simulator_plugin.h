#ifndef NOSTOP_SIMULATOR_H
#define NOSTOP_SIMULATOR_H
#ifdef ISSIMULATOR
#include <../plugins/abstract_simulator_plugin.h>
#include "nostop_router.hpp"
#include "nostop_packet.hpp"
#include <yaml_parser.h>
#include <types/world_sim_packet.h>
#include "../simulator/simulator.h"

namespace NoStop
{
	class DiscretizedArea;
  
	class Simulator_plugin : public abstract_simulator_plugin
	{
	public:
		Simulator_plugin( simulator* s );

		/// Get world information and initialize simulator
		bool initialize(Parsed_World const& w);

		/// Run NoStop Simulator plugin
		void run_plugin();

		/// Stop NoStop Simulator plugin
		void stop();

		/// 	
		//std::list<abstract_object*>* create_objects();
		
		///
		std::string get_objects_type() {return "NO_STOP";}
	private:

		std::shared_ptr<DiscretizedArea> m_area;

		int m_num_agents;

		bool m_router_started;

		std::shared_ptr< Router<Coverage_packet> > m_router;
		world_sim_packet sim_packet;
	};
}

#endif
#endif