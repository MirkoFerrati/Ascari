//written by Alessandro Settimi
#include "nostop_agent_plugin.h"
#include "nostop_agent_position.h"
#include "nostop_parser_plugin.h"
#include "nostop_plugin.hpp"
#include "nostop_thread.hpp"
#include "nostop_ISLAlgorithm.h"
#include "nostop_packet.hpp"
#include "nostop_discretized_area.h"
#include "nostop_probability.h"

#include "../agent/agent.h"

namespace NoStop 
{
	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::createDiscretizedArea(Parsed_World* world)
	{
		auto wo=reinterpret_cast<NoStop::Parsed_world*>(world->parsed_items_from_plugins[0]);
		//	Get vertex from Parsed World:
		std::vector<Real2D> l_vertices = wo->getExternalVertices();
		std::set< std::vector<Real2D> > l_obstacles = wo->getObstaclesVertices();
		int l_num_col = wo->getDiscretization();
		int l_num_row = wo->getDiscretization();

		m_area = std::make_shared<DiscretizedArea>(l_vertices, l_obstacles, l_num_col, l_num_row);
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::colletOtherAgentsInfo(Parsed_World* world)
	{
		auto wo=reinterpret_cast<NoStop::Parsed_world*>(world->parsed_items_from_plugins[0]);
		for (unsigned int i=0;i<wo->m_agents.size();i++)
			m_other_agents_id.push_back(wo->m_agents.at(i));
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::intializeLearningAlgorithm(Parsed_World* world)
	{
		m_algorithm = std::make_shared<ISLAlgorithm>(m_area, this);
		m_algorithm->initialize(world);
	}

	//////////////////////////////////////////////////////////////////////////
	Agent_plugin::Agent_plugin ( agent* a, Parsed_World* parse )
		:	m_time(a->time),
		m_id(a->identifier),
		m_area(nullptr),
		m_mutex(nullptr),
		m_initialized(false),
		m_events(a->events),
		m_events_to_index(a->events_to_index)
	{
		initialize(parse);
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::initialize(Parsed_World* world)
	{
		m_mutex = std::make_shared<std::mutex>();

		/// Collect agent initial position
		double x0=world->agents.front().initial_states.at("X");
		double y0=world->agents.front().initial_states.at("Y");
		m_position = AgentPosition( Real2D(x0, y0) );

		//	Create agent area Map
		createDiscretizedArea(world);

		// Get global informations about other agents
		colletOtherAgentsInfo(world);

		// Initialize ISL Algorithm
		intializeLearningAlgorithm(world);
	}

	//////////////////////////////////////////////////////////////////////////
	int Agent_plugin::getNumberOfNeighbors()
	{
		int tot = 0;
		for(size_t i= 0; i < m_other_agents_id.size(); ++i)
		{
			if(m_other_agents_id[i] != m_id)
			{
				std::map<Agent_ID, AgentPosition>::iterator it = m_other_agents_position.find(m_other_agents_id[i]);
				if( it != m_other_agents_position.end() )
					/// l'agente m_other_agents_id[i] esiste nella memoria di questo agente:
				{
					// Verifica se sta tra i vicini
					if( this->isNeighbor(it->second) )
					{
						tot++;
					}
				}
			}
		}
		return tot;
	}

	//////////////////////////////////////////////////////////////////////////
	int Agent_plugin::getNumberOfVisibleNeighbors()
	{
		int tot = 0;
		for(size_t i= 0; i < m_other_agents_id.size(); ++i)
		{
			if(m_other_agents_id[i] != m_id)
			{
				std::map<Agent_ID, AgentPosition>::iterator it = m_other_agents_position.find(m_other_agents_id[i]);
				if( it != m_other_agents_position.end() )
					/// l'agente m_other_agents_id[i] esiste nella memoria di questo agente:
				{
					// Verifica se sta tra i vicini
					if( this->isVisibleNeighbor(it->second) )
					{
						tot++;
					}
				}
			}
		}
		return tot;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Agent_plugin::isVisibleNeighbor(AgentPosition const& other)
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Agent_plugin::isNeighbor(AgentPosition const& other)
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::compute_speeds(double x_t,double y_t) //CONTROLLARE
	{
		double l_angle = Math::polarPhi2D( Real2D(x_t, y_t) - m_position.getPoint2D() );
		m_angSpeed = -1 * sin( m_position.getOrientation() - l_angle );

		switch(m_status)
		{
		case Agent_plugin::MOVING:
			m_tanSpeed = m_distance_to_target() > 2 ? 0.2 : m_distance_to_target() / 10.;
			break;
		case Agent_plugin::WAITING:
			m_tanSpeed = 0.;
			break;
		case Agent_plugin::SUSPENDING:
		default:
			m_tanSpeed = 0.;
			break;
		}
		double l_angSpeed;
		avoid_collision(l_angSpeed);

		double beta_p = -0.1;

		double lambda_u = 0.;
		lambda_u += beta_p * (m_angSpeed - l_angSpeed);

		if(fabs(m_angSpeed) > m_max_angSpeed) 
			m_angSpeed = copysign(m_max_angSpeed,m_angSpeed);
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::avoid_collision(double& _angSpeed) //CONTROLLARE
	{
		_angSpeed=0;
		double min_d=INF;
		for(unsigned int i=0;i<m_other_agents_id.size();i++)
		{
			if( m_other_agents_id.at(i) != m_id )
			{
				std::map<Agent_ID,AgentPosition>::iterator it = m_other_agents_position.find(m_other_agents_id.at(i));
				if(it == m_other_agents_position.end() )
					continue;
				double d = ( m_position.getPoint2D() - it->second.getPoint2D() ).sqrmod();
				min_d=std::min(d,min_d);
				if(d > 0) 
				{
					double l_angle = Math::polarPhi2D( it->second.getPoint2D() - m_position.getPoint2D() );
					_angSpeed += ( 2. / d ) * sin( m_position.getOrientation() - l_angle );
				}
			}
		}

		//speed = speed-((min_d<1.2)?0.15:0); //per rallentare vicino ad altri robot
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::receiveDataFromOthers()
	{
		// Open the door for communication and get data
		std::vector<Coverage_packet>& l_data_receive = *( (std::vector<Coverage_packet>*) m_communicator->get_data() );

		// Lock agent data
		m_mutex->lock();

		/// Reset old data in the area
		resetDiscretizedAreaCounter();

		// Collect data from the other visible agents
		for (unsigned int i=0;i<l_data_receive.size();i++)
		{
			Agent_ID l_name = l_data_receive.at(i).getAgentID();
			if ( /*( l_name != "" ) &&*/ !( l_name != m_id ) )
			{
				// with simulation time it's possible to select right packets
				simulation_time l_time = l_data_receive.at(i).getTime();

				ISLAlgorithm_packet* l_temp = (ISLAlgorithm_packet*) l_data_receive.at(i).getData();

				mergeReceivedData(l_temp);

				AgentPosition l_position = l_data_receive.at(i).getAgentPosition();
				m_other_agents_position[l_name] = l_position;
			}
		}

		l_data_receive.clear();

		m_mutex->unlock();
	}

	///
	void Agent_plugin::mergeReceivedData(ISLAlgorithm_packet* _data)
	{
		m_area->mergeReceivedData(_data);
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::run_plugin()
	{
		double l_dec= m_time - floor(m_time);

		if (!m_initialized)
		{
			if(m_algorithm)
			{
				bool l_fresh = false;
				// Initialize the packet necessary to communicate
				m_packet = std::make_shared<Coverage_packet>( 0., m_id, m_position, ISLAlgorithm_packet() );
				m_communicator = std::make_shared<Communicator>(
					m_mutex,
					m_packet.get(),
					m_other_agents_id.size(),
					m_id.str(),
					l_fresh);

				std::cout<<"NOSTOP ISL ALGORITHM"<<std::endl;
				m_initialized = true;
			}
			else
			{
				std::cout<<"Pay attention to configuration file, it is NOT for NoStop Plugin!"<<std::endl;;
			}
		}

		// Check if it is outside of the main area and if it is out, stop it (change agent status)
		if(0)
		{
			// CONTROLLARE
			std::cout<<"OUT OF THE MAIN AREA... SHUTDOWN..."<<std::endl;
			m_tanSpeed = 0.;
			m_angSpeed = 0.;
		}
		else
		{
			bool l_isANewTimeStep=(fabs(l_dec-0.0) < 0.001)||(fabs(l_dec-0.5) < 0.001);
			if(l_isANewTimeStep && m_status != MOVING)
			{
				std::cout<<"[NoStop time - "<<m_time<<']'<<std::endl;

				/// Receive data from other agents
				receiveDataFromOthers();

				// Set new target: m_target_x, m_target_y;
				m_algorithm->forwardOneStep(m_time, m_area, m_packet);
			}
			else
			{
				receiveDataFromOthers();
				m_packet.get()->m_pos = AgentPosition( Real2D(m_x.value(), m_y.value()), m_packet.get()->m_pos.getCameraPosition());
				m_packet.get()->m_pos.setOrientation( m_theta.value() );
			}

			m_communicator->send();

#ifdef _PRINT
			std::cout << "Send x:" << x.value() << " y:" << y.value() << " theta:" << theta.value() << std::endl;
#endif

			compute_speeds(m_nextPosition.getPoint2D()[0], m_nextPosition.getPoint2D()[1]);
			//std::cout<<"speed= "<<speed<<" omega= "<<omega<<" omega_dubins "<<omega_dubins<<" set_charge= "<<set_charge<<std::endl;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin ::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
	{
		symbol_table.add_variable("X_TARGET",m_nextPosition.m_point[0],false);
		symbol_table.add_variable("Y_TARGET",m_nextPosition.m_point[1],false);
		symbol_table.add_variable("V_T",m_tanSpeed,false);
		symbol_table.add_variable("W_T",m_angSpeed,false);
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin ::compileExpressions(exprtk::symbol_table< double >& _symbol_table)
	{
		exprtk::parser<double> l_parser;

		m_distance_to_target.register_symbol_table(_symbol_table);
		if (!l_parser.compile("sqrt((X_TARGET-X)^2+(Y_TARGET-Y)^2)",m_distance_to_target))
		{
			ERR("impossibile creare l'espressione: %s","sqrt(sqr(X_TARGET-X)+sqr(Y_TARGET-Y))");
			throw "impossibile creare l'espressione";
		}

		m_x.register_symbol_table(_symbol_table);
		if (!l_parser.compile("X",m_x))
		{
			ERR("impossibile creare l'espressione: %s","X");
			throw "impossibile creare l'espressione";
		}

		m_y.register_symbol_table(_symbol_table);
		if (!l_parser.compile("Y",m_y))
		{
			ERR("impossibile creare l'espressione: %s","Y");
			throw "impossibile creare l'espressione";
		}

		m_theta.register_symbol_table(_symbol_table);
		if (!l_parser.compile("THETA",m_theta))
		{
			ERR("impossibile creare l'espressione: %s","THETA");
			throw "impossibile creare l'espressione";
		}
	}

	//////////////////////////////////////////////////////////////////////////
	Agent_plugin::~Agent_plugin()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::resetDiscretizedAreaCounter()
	{
		Lock lock(m_mutex);
		if(m_area)
			m_area->resetCounter();
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::selectRandomFeasibleAction()
	{
		std::vector<AgentPosition> l_feasible = getFeasibleActions();
		if(l_feasible.empty())
			setNextPosition();
		else
		{
			int l_value = getRandomValue( int( l_feasible.size() ) );
			setNextPosition(l_feasible[l_value]);
		}
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::setNextPosition(AgentPosition const& pos)
	{
		Lock lock(m_mutex);
		m_nextPosition = pos;
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::setNextPosition()
	{
		Lock lock(m_mutex);
		m_nextPosition = m_position;
	}

	//////////////////////////////////////////////////////////////////////////
	std::vector<AgentPosition> Agent_plugin::getFeasibleActions()
	{
		AreaCoordinate l_currCoord = m_area->getCoordinate( m_position.getPoint2D() );

		std::vector<AreaCoordinate> l_squares = m_area->getStandardApproachableValidSquares(l_currCoord);

		std::vector<AgentPosition> l_result;
		for( size_t i = 0; i < l_squares.size(); ++i )
		{
			l_result.push_back( AgentPosition( m_area->getPosition( l_squares[i] ), m_position.m_camera ) );
		}

		return l_result;
	}

	//////////////////////////////////////////////////////////////////////////
	std::set<std::shared_ptr<Square> > Agent_plugin::getVisibleSquares( )
	{
		std::set<std::shared_ptr<Square> > result;
		std::vector<AreaCoordinate> l_coord = m_position.getCoverage(m_area);
		for(size_t i = 0; i < l_coord.size(); ++i)
		{
			result.insert(m_area->getSquare(l_coord[i]));
		}
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	void Agent_plugin::setCurrentPayoff( double l_benefit )
	{
		m_currentPayoff = l_benefit;
	}
}