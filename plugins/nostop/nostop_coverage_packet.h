#ifndef NOSTOP_COVERAGE_PACKET_H
#define NOSTOP_COVERAGE_PACKET_H

#include "nostop_agent_plugin.h"
#include "nostop_agent_id.h"
#include "../shared/types.h"

namespace NoStop
{  
	template <typename data_type>
	class NoStop_packet
	{
	private:
		simulation_time m_time;
		Agent_ID m_agent_id;
		AgentPosition m_pos;
		data_type m_data;

	public:
		NoStop_packet(simulation_time _time, Agent_ID _agent_id, AgentPosition _pos, data_type _data)
			: m_time(_time), m_agent_id(_agent_id), m_pos(_pos), m_data(_data)
		{}

		void setData(void *_data)
		{
			m_data=*(data_type*)_data;
		}

		void* getData()
		{
			return &m_data;
		}

		void* getTime()
		{
			return &m_time;
		}

		void* getAgentPosition()
		{
			return &m_pos;
		}
		
		void* getAgentID()
		{
			return &m_agent_id;
		}

		template <typename Archive>
		void serialize(Archive& ar,const unsigned int /*version*/)
		{
			ar& m_time;
			ar& m_agent_id;
			ar& m_pos;
			ar& m_data;
		}
	};

	struct ISLAlgorithm_packet
	{
		std::vector<int> area;

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int /*version*/)
		{
			ar& area;
		}
	};

	typedef NoStop_packet<ISLAlgorithm_packet> Coverage_packet;

}

#endif