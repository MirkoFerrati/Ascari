//written by Simone Nardi
#ifndef NOSTOP_AGENT_PLUGIN_H
#define NOSTOP_AGENT_PLUGIN_H

#include "../HACK_KDEVELOP.h"
#ifdef ISAGENT

#include <vector>

#include <yaml_parser.h>

#include "nostop_agent_position.h"

#include "nostop_communicator.hpp"
#include "nostop_coverage_packet.h"
#include <random.hpp>
#include <types/events.h>

#include "../shared/types.h"
#include "../plugins/abstract_agent_plugin.h"
#include "../agent/agent.h"

#include <set>

namespace NoStop
{
	class ISLAlgorithm;
  
	/// Struct representing memory of an agent
	struct MemoryAgentPosition
	{
		AgentPosition m_action;
		double m_payoff;

		MemoryAgentPosition(AgentPosition const& _action, double _payoff) : m_action(_action), m_payoff(_payoff) {}
	};

	/// Class representing a NoStop Agent
	class Agent_plugin: public abstract_agent_plugin
	{
		///	Global time of the simulation
		simulation_time& m_time;

		///	The Status of the agent
		enum Status 
		{
			MOVING = 0,
			WAITING = 1,
			COMPUTING = 2,
			SUSPENDING = 3
		} m_status;

		///	Agent Identifier
		Agent_ID m_id;

		/// Global area of the world
		std::shared_ptr<DiscretizedArea> m_area;

		/// Mutex
		std::shared_ptr<std::mutex> m_mutex;

		/// Communication from agent to simulator
		std::shared_ptr< Communicator<Coverage_packet,Coverage_packet> > m_communicator;

		/// Coverage packet
		std::shared_ptr<Coverage_packet> m_packet;

		/// Agent Position in space
		AgentPosition m_position;

		/// Current payoff received in the current position
		double m_currentPayoff;

		/// Agent next Position in space
		AgentPosition m_nextPosition;

		/// The physics memory of the agent
		std::vector<MemoryAgentPosition> m_memory;

		/// Memory dimension
		int m_memorySpace;

		bool m_initialized;

		std::shared_ptr<ISLAlgorithm> m_algorithm;

		//////////////////////////////////////////////////////////////////////////

		/// Tangential Speed 
		double m_tanSpeed;
		/// Angular Speed
		double m_angSpeed;
		/// Maximum value of the angular speed
		double m_max_angSpeed = 0.2;

		exprtk::expression<double> m_distance_to_target;
		exprtk::expression<double> m_x;
		exprtk::expression<double> m_y;
		exprtk::expression<double> m_theta;

		double m_target_x, m_target_y;

		std::map< transition, Events >& m_events;
		const std::map<std::string,transition>& m_events_to_index;

		//////////////////////////////////////////////////////////////////////////
		/// NOT NECESSARY PARAMETERS FOR DISTRIBUTED ALGORITHM

		/// List of other agents
		std::vector<Agent_ID> m_other_agents_id;

		/// Link between agents and their positions
		std::map<Agent_ID, AgentPosition> m_other_agents_position;

	protected:
		/// Initialize world variable to create discretization map and get Number of other agents
		void initialize(Parsed_World* parse);

		/// Compute Speed to avoid collisions
		void compute_speeds(double x_t,double y_t);

		/// Compute direction to avoid collisions
		void avoid_collision(double& a);

		/// Merge received data with existing data
		void mergeReceivedData(Coverage_packet* _packet);

		/// Reset data in the discretized area
		void resetDiscretizedAreaCounter();

		/// True if the position is in the visibility range of this agent
		bool isVisibleNeighbor(AgentPosition const& other);

		/// True if the position is in the communication range of this agent
		bool isNeighbor(AgentPosition const& other);

		/// Create area 
		void createDiscretizedArea(Parsed_World* parse);

		/// Collect other agents information (IDs and positions)
		void colletOtherAgentsInfo(Parsed_World* parse);

		/// Receive packet from other agents
		void receiveDataFromOthers();

		/// Initialize Learning Algorithm (ISL)
		void intializeLearningAlgorithm(Parsed_World* parse);

		/// Set next target position
		void setNextPosition(AgentPosition const& pos);

		/// Set next target position
		void setNextPosition();

		/// Get set of feasible actions
		std::vector<AgentPosition> getFeasibleActions();

		/// The set of square can be controlled by this agent
		std::set<std::shared_ptr<Square> > getVisibleSquares( );

	public:

		/// Constructor
		Agent_plugin(agent* a, Parsed_World* parse);

		/// inherit from abstract agent
		void addReservedVariables(exprtk::symbol_table< double >& symbol_table_reference);
		/// inherit from abstract agent
		void compileExpressions(exprtk::symbol_table< double >& symbol_table_reference);
		/// run of the agent, inherit from abstract agent
		void run_plugin();

	public:
		/// Compute the number of Neighbors
		int getNumberOfNeighbors();

		/// Compute the number of Visible Neighbors
		int getNumberOfVisibleNeighbors();

		/// Get Simulation Time
		simulation_time getSimulationTime() {return m_time;}

		void setCurrentPayoff( double l_benefit );
	};
}
#endif

#endif // NOSTOP_AGENT_PLUGIN_H
