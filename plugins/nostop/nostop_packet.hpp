#ifndef NOSTOP_PACKET_H
#define NOSTOP_PACKET_H

#include "nostop_agent_plugin.h"
#include "nostop_agent_id.h"
#include "nostop_ISLAlgorithm_packet.h"
#include "../shared/types.h"

namespace NoStop
{  
	template <typename data_type>
	class Packet
	{
	public:
		simulation_time m_time;
		Agent_ID m_agent_id;
		AgentPosition m_pos;
		data_type m_data;

	public:
		Packet()
			: m_time(), m_agent_id(""), m_pos(), m_data()
		{}
	  
		Packet(simulation_time _time, Agent_ID _agent_id, AgentPosition _pos, data_type _data)
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

		simulation_time getTime()
		{
			return m_time;
		}

		AgentPosition getAgentPosition()
		{
			return m_pos;
		}
		
		Agent_ID getAgentID()
		{
			return m_agent_id;
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
	
	typedef Packet<ISLAlgorithm_packet> Coverage_packet;
}

#endif