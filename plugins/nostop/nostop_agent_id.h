#ifndef NOSTOP_AGENT_ID
#define NOSTOP_AGENT_ID

namespace NoStop
{
	/// Agent Identifier, name and team
	class Agent_ID
	{
		int m_id;
		int m_team;
	public:
		/// Construct Identifier from configuration file string
		Agent_ID(std::string const& name);

		/// Get the name of the agent
		int getID() const;

		/// Get the name of the team
		int getTeamID() const;

		/// Operator minor to use Agent_ID as a key in STL container
		bool operator<(Agent_ID const& other) const;
	};
}

#endif