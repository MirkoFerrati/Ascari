#ifndef NOSTOP_VIEWER_PLUGIN_H
#define NOSTOP_VIEWER_PLUGIN_H
#include "../plugins/abstract_viewer_plugin.h"

#ifdef ISVIEWER

#include "yaml_parser.h"
#include <boost/circular_buffer.hpp>
#include "viewer.h"

namespace NoStop
{
	/// The Viewer class to add feature to the window
	class Viewer_plugin:public abstract_viewer_plugin
	{
	public:
		Viewer_plugin();
		Viewer_plugin ( std::shared_ptr< std::mutex >& mutex, const world_sim_packet& infos );
		void timerEvent(std::shared_ptr<std::mutex>& mutex, const world_sim_packet& infos);
		void paintBackground ( QPainter& painter );
		void paintAgents ( QPainter& painter, const std::map<std::string, Agent>& agents );  
		~Viewer_plugin();

	private:
		bool m_started;
		double m_old_time, m_now_time;
	};
}

#endif
#endif // NOSTOP_VIEWER_PLUGIN_H
