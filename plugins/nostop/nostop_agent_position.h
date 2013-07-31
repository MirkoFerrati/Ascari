#ifndef NOSTOP_AGENT_POSITION_H
#define NOSTOP_AGENT_POSITION_H

#include "nostop_math.h"
#include "nostop_real2D.h"

#include <vector>
#include <memory>

namespace NoStop
{
	class DiscretizedArea;
	struct AreaCoordinate;
	class Square;
	class CameraArea;
	class AgentPosition;

	class CameraArea
	{
		/// Center.
		Real2D m_center;
		/// Radius.
		double m_farRadius;
	public:
		bool contains(Real2D const& pt) const;

		CameraArea(const Real2D & _center, const double & _farRad) : m_center(_center), m_farRadius(_farRad) {}
	};

	//////////////////////////////////////////////////////////////////////////
	class CameraPosition
	{
	protected:
		/// Major radius.
		double m_farRadius;
		/// Minor Radius.
		double m_nearRadius;
		/// Orientation.
		double m_orientation; // [0, 2*Pi)
		/// Angle of view.
		double m_angle;

	public:
		CameraPosition(double _farRadius = 0., double _nearRadius = 0., double _orientation = 0., double _angle = Math::TwoPi) 
			: m_farRadius(_farRadius), m_nearRadius(_nearRadius), m_orientation(_orientation), m_angle(_angle) {}

		std::vector<AreaCoordinate> getCoverage(AreaCoordinate _center, std::shared_ptr<DiscretizedArea> _area) const;

		double getFarRadius() const {return m_farRadius;}

		double getOrientation() const {return m_orientation;}

		std::shared_ptr<CameraArea> getVisibleArea(Real2D const& point) const;

		double computeCosts() const {return 0.;}

		void setOrientation(double const& _orientation) { m_orientation = _orientation;}

		friend class AgentPosition;

		template <typename Archive>
		void serialize(Archive& ar,const unsigned int /*version*/)
		{
			ar& m_farRadius;
			ar& m_nearRadius;
			ar& m_orientation;
			ar& m_angle;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	class AgentPosition
	{
	public:
		/// The position of the agent.
		Real2D m_point;

		/// The camera orientation.
		CameraPosition m_camera;

	public:
		AgentPosition() {};

		AgentPosition(Real2D const& point, double const& _orientation) : m_point(point), m_camera() 
		{ m_camera.m_orientation = _orientation; }

		AgentPosition(Real2D const& point, CameraPosition _camera = CameraPosition() ) : m_point(point), m_camera(_camera) {}

		/// Update the counter of the lattice visible from that position
		void updateCounter(std::shared_ptr<DiscretizedArea> area);

		/// Get Point2D
		Real2D getPoint2D() const {return m_point;}

		/// Compute all the square visible by that position and that camera
		bool visible(std::shared_ptr<Square> _area) const;

		/// Compute Camera Costs
		double computeCosts() const;

		std::vector<AreaCoordinate> getCoverage(std::shared_ptr<DiscretizedArea> _space ) const;

		std::shared_ptr<CameraArea> getVisibleArea() const;

		double getOrientation() const {return m_camera.getOrientation(); }

		void setOrientation(double const& _orientation) { m_camera.setOrientation(_orientation); }

		CameraPosition getCameraPosition() {return m_camera;}

		template <typename Archive>
		void serialize(Archive& ar,const unsigned int /*version*/)
		{
			ar& m_point[0];
			ar& m_point[1];
			ar& m_camera;
		}
	};
}
#endif