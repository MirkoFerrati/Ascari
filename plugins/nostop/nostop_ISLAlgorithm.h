///////////////////////////////////////////////////////////
//  nostop_ISLAlgorithm.h
//  Created on:      13-may-2013 10.14.52
//  Original author: s.nardi
///////////////////////////////////////////////////////////
#ifndef NOSTOP_ISL_ALGORITHM_H
#define NOSTOP_ISL_ALGORITHM_H
#pragma once

#include "../shared/types.h"

//	Coverage
#include  "nostop_packet.hpp"

#include <memory>
#include <set>

class Parsed_World;

namespace NoStop
	{
		class Agent_plugin;
		class DiscretizedArea;

		/**
		*	\brief	Inhomogeneous Synchronous Learning Algorithm
		*	see M.Zhu and S.Martinez, 
		*	Distributed coverage game for mobile visual sensors (I): Reaching the set of Nash equilibria.
		*	in Proceedings of the 48th IEEE Conference on Decision and Control.
		*/
		class ISLAlgorithm 
		{
			/// The agent owner of the algorithm
			Agent_plugin* m_agent;

			/// The area of the world
			std::shared_ptr<DiscretizedArea> m_area;
			
			/// Number of the guards in the world
			int m_numberOfGuards;
		
		protected:

			/// Update action of the agent
			void update();

			/// Update maps with visible square of the agent
			void updateCounterOfVisibleSquare();

			/// Compute exploration rate
			double computeExplorationRate();

			/// True if agent can experiment a new action
			bool agentHasToExperiments();

			/// Compute Payoff
			void compute();

		public:
			/// Constructor
			ISLAlgorithm(std::shared_ptr<DiscretizedArea> _space, Agent_plugin* _agent);

			/// Initialize algorithm
			void initialize(Parsed_World* parse);

			/// Compute one step movement
			void forwardOneStep( 
				simulation_time & _time, 
				std::shared_ptr<DiscretizedArea> _area, 
				std::shared_ptr<Coverage_packet> _packet);

		};
}
#endif
