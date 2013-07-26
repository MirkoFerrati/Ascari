#include "nostop_agent_id.h"

namespace NoStop
{
	/// 
	std::vector<std::string> tokenize(std::string const& name)
	{
		std::vector<std::string> l_res;
		char_separator<char> sep(", ");
		tokenizer<char_separator<char>> tokens(name, sep);
		for (const auto& t : tokens) 
		{
		    l_res.push_back(t);
		}
		return l_res;
	}
  
	/// Construct Identifier from configuration file string
	Agent_ID::Agent_ID(std::string const& name)
	{
		std::vector<std::string> l_value = tokenize(name);
	  	  
		m_team = 1;
		for(size_t i = 0; i < l_value.size(); ++i)
		{
		  if(i == 1)
		    m_id = atoi(l_value[i]);
		  else if(i == 3)
		    m_team == l_value[i];
		}
	}
	
	///
	std::string Agent_ID::str() const
	{
	    std::stringstream out;
	    out << "Agent_" << m_id << "_Team_" << m_team;
	    return out;
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
	
	///
	bool Agent_ID::operator!=(Agent_ID const& other) const
	{
	  if(other < *this)
	    return true;
	  else if(*this < other)
	    return true;
	  else
	    return false;
	}
	
	///
	bool Agent_ID::operator==(Agent_ID const& other) const
	{
	  return !(other!=*this);
	}
  
}