#include "nostop_line2D.h"

#include <set>

namespace NoStop {

		Line2D::Line2D()
			: m_origin(), m_direction(0,1) {}

		Line2D::Line2D(const Real2D& origin,const Real2D& direction)
			: m_origin(origin), m_direction(direction.vers())
		{
			if (this->m_direction.sqrmod() < 0.5)
				throw std::exception();
		}
		/**
		*  Contruct a line with the given origin and direction.
		*/
		Line2D::Line2D(const Real2D& origin,const Real2D& target)
		{
			m_origin = origin;
			m_direction = (target - origin).vers();

			if (this->m_direction.sqrmod() < 0.5)
				throw std::exception();
		}

		/**
		*  Contruct a line with the given origin and direction.
		*/
		Line2D::Line2D(const Real2D& origin, double direction)
		{
			m_origin = origin;
			m_direction = Real2D(0,1).rotateRad(Math::toRad(-direction));
		}

		const Real2D& Line2D::origin() const
		{
			return this->m_origin;
		}

		const Real2D& Line2D::direction() const
		{
			return this->m_direction;
		}

		/**
		*	Get the bearing of the line.
		*/
		double Line2D::azimuth() const
		{
			double angle  = Math::polarPhi2D(this->m_direction[0],this->m_direction[1]);
			angle = 90 - Math::toDeg(angle);
			while (angle >= 360) angle -= 360;
			while (angle < 0) angle += 360;
			if(angle == 360) angle -= 360;
			return angle;
		}

		// Line parameter at point.
		double Line2D::parameterAt(const Real2D& point) const
		{
			Real2D ext = point - this->origin();
			return ext * this->direction();
		}

		Line2D Line2D::move(double distance) const
		{
			return Line2D(this->origin() + this->direction()*distance,
				this->direction());
		}

		Line2D Line2D::moveTo(const Real2D& point) const
		{
			return Line2D(point, this->direction());
		}

		Real2D Line2D::pointFromOrigin(double distance) const
		{
			return this->origin() + this->direction()*distance;
		}

		Line2D Line2D::rotate(double angle) const
		{
			return Line2D(this->origin(),
				this->direction().rotateRad(Math::toRad(-angle)));
		}

		bool pointOnLine(
			const Real2D& origin,
			const Real2D& dir,
			const Real2D& point,
			/*out*/ double& param)
		{
			Real2D d = point - origin;

			Real2D ddir = dir.vers();
			double lateralDist = d ^ ddir;
			if (fabs(lateralDist) > Math::TOLERANCE)
				return false;

			param = d*ddir;
			return true;
		}

		bool Line2D::contains(const Real2D& point) const
		{
			double param;
			return pointOnLine(this->origin(), this->direction(), point, param);
		}

		int Line2D::getPointSide(const Real2D& point) const
		{
			Real2D d = point - origin();
			double dist = direction() ^ d;

			if (fabs(dist) < Math::Math::TOLERANCE) return 0;
			else if (dist > 0) return 1;
			else return -1;
		}

		int intersect(
			const Real2D& firstDir, const Real2D& firstOrigin, /*out*/ double& firstParam,
			const Real2D& secondDir, const Real2D& secondOrigin, /*out*/ double& secondParam,
			Real2D& point)
		{
			Real2D p = secondOrigin - firstOrigin;
			/// Computes the normal vector of the first line.
			Real2D n1 = firstDir.normal();
			/// Computes the normal vector of the second line.
			Real2D n2 = secondDir.normal();

			double sinAngle = firstDir*n2;
			// Fabbri: two straight lines are considered as "parallel" when their directions
			//differ by 1 micron every 1000km!
			if (fabs(sinAngle) < 1.E-12)
				return 0;

			/// Computes t1.
			firstParam = (p*n2)/sinAngle;
			if(!_finite(firstParam))
				return 0;
			/// Computes t2.
			secondParam = (p*n1)/sinAngle;
			if(!_finite(secondParam))
				return 0;

			point = Real2D( (firstOrigin + firstParam*firstDir + secondOrigin + secondParam*secondDir) / 2. );
			//point = Real2D(firstOrigin + firstParam*firstDir);
			return 1;
		}

		int intersect(
			const Line2D& first, /*out*/ double& firstParam,
			const Line2D& second, /*out*/ double& secondParam,
			Real2D& point)
		{
			Real2D firstDir = first.direction();
			Real2D firstOrigin = first.origin().coord();
			Real2D secondDir = second.direction();
			Real2D secondOrigin = second.origin().coord();

			return 	intersect(
				firstDir, firstOrigin, firstParam,
				secondDir, secondOrigin, secondParam,
				point);
		}

		vector<Real2D> Line2D::intersect(const Line2D& second) const
		{
			double firstParam, secondParam;
			Real2D point;
			vector<Real2D> points;

			if( intersect( *this, firstParam, second, secondParam, point) )
				points.push_back(point);

			return points;
		}

		///------------------------------------------------------
		bool Line2D::intersect(const Box& box) const
		{
			//return true; //CONTROLLARE Rallenta;
			std::set<int> sided;
			for(int i = 0; i < 4;i++)
			{
				int l_side = this->getPointSide(box.corner(i));

				if(l_side == 0)
					return true;

				if ( sided.find(l_side) == sided.end() && !sided.empty() )
					return true;
				else
					sided.insert(l_side);
			}
			return false;
		}

		// Project point onto line.
		Real2D Line2D::projectPoint(const Real2D& point) const
		{
			Real2D ext = point - this->origin();
			double param = ext * this->direction();

			return Real2D( this->origin() + param * this->direction() );
		}

		// Get the angle between this line and a target line.
		double Line2D::angleBetween(const Line2D& target_line) const
		{
			double a = Math::polarPhi2D(this->m_direction[0],this->m_direction[1]);
			double b = Math::polarPhi2D(target_line.m_direction[0],target_line.m_direction[1]);
			double diff = a - b;
			while (diff > Math::Pi) diff -= Math::TwoPi;
			while (diff <= -Math::Pi) diff += Math::TwoPi;
			return Math::toDeg(diff);
		}

		//	---------------------------------------------------------------------------------
		bool Line2D::isForward(const Real2D& point) const  
		{
			return  this->parameterAt(point) > -Math::Math::TOLERANCE;
		}

		//	---------------------------------------------------------------------------------
		bool Line2D::isBackward(const Real2D& point) const  
		{
			return  this->parameterAt(point) < Math::Math::TOLERANCE;
		}

		Line2D makeLine(const Real2D& origin, double angle)
		{
			return Line2D(origin, angle);
		}

		Line2D makeLine(const Real2D& origin, const Real2D& target)
		{
			return Line2D(origin, target);
		}

	}} // close namespace IDS::BaseGeometry
