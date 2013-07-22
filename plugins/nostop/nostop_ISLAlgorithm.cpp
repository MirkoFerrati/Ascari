#include "nostop_ISLAlgorithm.h"
#include "nostop_discretized_area.h"
#include "nostop_agent_plugin.h"
#include "nostop_probability.h"
#include "nostop_coverage_utility.h"

#include <sstream>
#include <string>

using namespace std;

namespace NoStop
{
	//////////////////////////////////////////////////////////////////////////
	ISLAlgorithm::ISLAlgorithm(std::shared_ptr<DiscretizedArea> _space, Agent_plugin* _agent) 
		: m_agent(_agent), m_space(_space), m_numberOfGuards(1)
	{}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::initialize(Parsed_World* parse)
	{
		auto wo=reinterpret_cast<Parsed_world*>(world.parsed_items_from_plugins[0]);    
		m_numberOfGuards = wo->agents.size() - 1; // per ora un solo agente nocivo, CONTROLLARE
	}

	//////////////////////////////////////////////////////////////////////////
	std::string ISLAlgorithm::getExplorationRate()
	{
		double l_exploration = this->computeExplorationRate();
		std::ostringstream strs;
		strs << l_exploration;
		return strs.str();
	}

	//////////////////////////////////////////////////////////////////////////
	double ISLAlgorithm::computeExplorationRate()
	{
		//return 0.5;
		double rate = ( /*double(m_guards.size())* */
			(double( m_area->getNumCol() ) + double( m_area->getNumRow() ) + 1) );
		return pow(double( m_agent->getSimulationTime(), -double( m_numberOfGuards ) / rate);
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::update()
	{
		static int count = 0;
		//	ogni agente guardia sceglie il proprio tasso di esplorazione:
		double l_explorationRate = computeExplorationRate();

		//	ogni agente guardia estrae se sperimentare nuove azioni o no
		bool l_agentHasToExperiments = agentHasToExperiments(l_explorationRate);
		if(l_agentHasToExperiments)
		{
			m_agent->selectRandomFeasibleAction();
		}
		else
		{
			m_agent->selectBestMemoryAction();
		}

		return;
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::compute()
	{
		//	ogni agente guardia identifica la propria utilità:
		double l_benefit = 0;
		std::set<std::shared_ptr<Square> > l_squares = m_agent->getVisibleSquares(m_area);
		for(std::set<std::shared_ptr<Square> >::iterator it = l_squares.begin(); it != l_squares.end(); ++it)
		{
			SquarePtr l_square = *it;
			int l_nq = l_square->getTheNumberOfAgent();
			double l_value = l_square->getValue();
			l_benefit += l_value / double(l_nq);
		}
		l_benefit -= m_agent->computeCosts();

		m_agent->setCurrentPayoff(l_benefit);

		return;
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::updateCounterOfVisibleSquare( )
	{
		std::set<SquarePtr> l_visible = m_agent->getVisibleSquares(m_area);
		SquarePtr l_currentSquare = m_area->getSquare( m_area->getCoordinate(_agent->getPosition().getPoint2D()) );
		for(std::set<SquarePtr>::iterator it = l_visible.begin(); it != l_visible.end(); ++it)
		{
			(*it)->increaseCounter();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::forwardOneStep(
		simulation_time & _time, 
		std::shared_ptr<DiscretizedArea> _area, 
		std::shared_ptr<Coverage_packet> _packet)
	{
		// Evaluate payoff of the given position
		evaluatePayoff();

		// Update action of the agent
		update();

				// Update maps with visible square of the agent
		updateCounterOfVisibleSquare();
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::selectBestMemoryAction()
	{
		std::vector< MemoryAgentPosition > l_memory = m_agent->getMemory();
		m_agent->setNextPosition(l_memory);
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::resetCounter()
	{
		m_space->resetCounter();
		m_space->resetValue();
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::getGuardsPosition(std::vector<AgentPosition> & _pos)
	{
		_pos.clear();
		_pos.reserve(m_guards.size());
		for(set<AgentPtr>::iterator it = m_guards.begin(); it != m_guards.end(); ++it)
		{
			AgentPtr l_agent = *it;
			if(l_agent->isGuard())
			{
				_pos.push_back( l_agent->getPosition() );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::getGuardsSquare(std::vector<SquarePtr> & _pos)
	{
		_pos.clear();
		_pos.reserve(m_guards.size());
		for(set<AgentPtr>::iterator it = m_guards.begin(); it != m_guards.end(); ++it)
		{
			AgentPtr l_agent = *it;
			if(l_agent->isGuard())
			{
				_pos.push_back( m_space->getSquare( l_agent->getPosition().getPoint2D() ) );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void ISLAlgorithm::getGuardsCoverage( std::vector< std::vector<IDS::BaseGeometry::Point2D> > & _areas)
	{
		for(set<AgentPtr>::iterator it = m_guards.begin(); it != m_guards.end(); ++it)
		{
			std::vector<IDS::BaseGeometry::Point2D> l_agentArea;
			AgentPtr l_agent = *it;
			Shape2D l_area = l_agent->getVisibleArea();
			std::vector<Curve2D> l_bound = l_area.getBoundary();
			for(size_t i = 0 ; i < l_bound.size(); ++i)
			{
				std::vector<IDS::BaseGeometry::Point2D> l_partial = l_bound[i].approxByPoints(1);
				l_agentArea.insert(l_agentArea.end(), l_partial.begin(), l_partial.end());
			}
			_areas.push_back(l_agentArea);
		}
	}

	double ISLAlgorithm::getPotentialValue()
	{
		double l_total = 0.;
		for(size_t i = 0;  i < m_space->m_lattice.size(); ++i)
		{
			double l_partialTotal = 0.;
			double l_Wq = m_space->m_lattice[i]->getValue();
			for(int j = 1; j <= m_space->m_lattice[i]->getTheNumberOfAgent(); ++j)
			{
				l_partialTotal += l_Wq / j;
			}
			l_total += l_partialTotal;
		}
		return l_total;
	}

}
