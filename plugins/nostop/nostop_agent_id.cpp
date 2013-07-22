#include "nostop_agent_id.h"

namespace NoStop
{
	/// Construct Identifier from configuration file string
	Agent_ID::Agent_ID(std::string const& name)
	{
		static int num= 0;
		num++;
		m_id = num;
		m_team = 1;
	}

	/// Get the name of the agent
	int Agent_ID::getID() const
	{
		return m_id;
	}

	/// Get the name of the team
	int Agent_ID::getTeamID() const
	{
		return m_team;
	}

	/// Operator minor to use Agent_ID as a key in STL container
	bool Agent_ID::operator<(Agent_ID const& other) const
	{
		if(m_team < other.getTeamID())
			return true;
		else if(m_team > other.getTeamID())
			return false;
		else if(m_id > other.getID())
			return true;

		return false;
	}
  
}