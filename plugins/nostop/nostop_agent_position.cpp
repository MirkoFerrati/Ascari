#include <memory>

#include "nostop_agent_position.h"
#include "nostop_discretized_area.h"
#include "nostop_math.h"

using namespace std;

namespace NoStop
{
	//////////////////////////////////////////////////////////////////////////
	bool CameraArea::contains(Real2D const& pt) const
	{
		return fabs(m_farRadius - (m_center - pt).mod() );
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	AgentPosition::AgentPosition()
	{}

	
	//////////////////////////////////////////////////////////////////////////
	void AgentPosition::updateCounter(std::shared_ptr<DiscretizedArea> area)
	{
		AreaCoordinate l_coord = area->getCoordinate(m_point);

		area->getSquare( l_coord.row, l_coord.col )->increaseCounter();

		// Calcolo la copertura in base alla camera:
		// per ora conta solo il raggio massimo e non cala con la distanza!
		std::vector<AreaCoordinate> l_cover = m_camera.getCoverage(l_coord, area);

		for(size_t i = 0; i < l_cover.size(); ++i)
		{
			std::shared_ptr<Square> l_square = area->getSquare( l_cover[i].row, l_cover[i].col );
			if(l_square)
				l_square->increaseCounter();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool AgentPosition::visible(std::shared_ptr<Square> _area) const
	{
		std::shared_ptr<CameraArea> sh = m_camera.getVisibleArea(m_point);
		if(sh)
			return sh->contains( _area->getBoundingBox().center() );
		else
			return false;
	}

	//////////////////////////////////////////////////////////////////////////
	double AgentPosition::computeCosts() const
	{
		return m_camera.computeCosts();
	}

	//////////////////////////////////////////////////////////////////////////
	std::vector<AreaCoordinate> AgentPosition::getCoverage(std::shared_ptr<DiscretizedArea> _space ) const
	{
		AreaCoordinate l_center = _space->getCoordinate(m_point);
		return m_camera.getCoverage(l_center, _space);
	}

	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<CameraArea> AgentPosition::getVisibleArea() const
	{
		return m_camera.getVisibleArea(m_point);
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	std::vector<AreaCoordinate> CameraPosition::getCoverage(AreaCoordinate _center, std::shared_ptr<DiscretizedArea> _area) const
	{
		Real2D l_pt = _area->getPosition(_center);
		std::shared_ptr<CameraArea> l_sensorArea = this->getVisibleArea(l_pt);
		if(!l_sensorArea)
			return std::vector<AreaCoordinate>();

		int l_rowDelta = floor(m_farRadius / _area->getYStep()) + 1;
		int l_colDelta = floor(m_farRadius / _area->getXStep()) + 1;

		std::vector<AreaCoordinate> result;
		AreaCoordinate l_elem;
		for(int i = - l_rowDelta; i <= l_rowDelta; ++i)
		{
			int row = _center.row + i;
			if( row < 0 || row > _area->getNumRow() )
				continue;
			for(int j = - l_colDelta; j <= l_colDelta; ++j)
			{
				int col = _center.col + j;
				if( col < 0 || col > _area->getNumRow() )
					continue;

				SquarePtr l_square = _area->getSquare(row, col);
				if(!l_square)
					continue;

				Real2D l_center = l_square->getBoundingBox().center();
				if( l_sensorArea->contains(l_center) )
				{
					l_elem.row = row;
					l_elem.col = col;
					result.push_back(l_elem);
				}
			}
		}
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<CameraArea> CameraPosition::getVisibleArea(Real2D const& point) const
		try
	{
		if(fabs(m_farRadius) < Math::TOLERANCE)
			return nullptr;

		return std::make_shared<CameraArea>( point, m_farRadius );
	}
	catch (...)
	{
		return nullptr;
	}
}
