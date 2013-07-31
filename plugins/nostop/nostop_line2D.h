///////////////////////////////////////////////////////////
//  nostop_line2D.h
//  Implementation of the Class Line2D
///////////////////////////////////////////////////////////
#if !defined NOSTOP_LINE2D_H
#define NOSTOP_LINE2D_H

#include "nostop_real2D.h"

namespace NoStop {

	class Box;
	/**
	*  Straight line in the Euclidean plane.
	*
	*  The origin is represented as a 2D vector, and the direction
	*  as a normalized 2D vector.
	*/
	class Line2D
	{

	public:
		/**
		*	Default constructor.
		*/  
		Line2D();

		/**
		*  Contruct a line with the given origin and direction.
		*
		*	@param origin [in] The origin of the line.
		*	@param target [in] A point on the line. If target == origin an exception of type
		*  DegenerateGeometry will be thrown.
		*/
		Line2D(const Real2D& origin,const Real2D& target);

		/**
		*  Contruct a line with the given origin and direction.
		*
		*	@param origin [in]		The origin of the line.
		*	@param direction [in]	(DECIMAL DEGREES) The direction of the line. 
		*							 Measured from y axes, in clockwise direction.
		*/
		Line2D(const Real2D& origin, double direction);

		/**
		*	Get the origin of the line.
		*
		*	@return The origin of the line.
		*/
		const Real2D& origin() const;

		/**
		*	Get the direction of the line.
		*
		*	@return The direction of the line (normalized).
		*/
		const Real2D& direction() const;

		/**
		*	Get the azimuth of the line.
		*
		*	@return (DECIMAL DEGREES) The azimuth of the line, measured in degrees in clockwise direction,
		*	between [0, 360).
		*/
		double azimuth() const;

		/**
		*	Get the parameter at a given point.
		*	If the point is not on the line, the projection of the point on
		*  the line will be considered instead.
		*
		*	@param point [in] The test point.
		*
		*	@return (METERS) The parameter at the test point. The absolute value
		*    of the parameter is equal to the distance between the point and the
		*    origin of the line. The return value is positive if the point is
		*    after the origin, negative otherwise.
		*/
		double parameterAt(const Real2D& point) const;

		/**
		*	Move the line. 
		*	A positive distance moves the origin forward, a negative
		*	distance moves the origin backward.
		*
		*	@param distance [in] (METERS) The distance.
		*
		*	@return A new line, obtained by moving along this line.
		*/
		Line2D move(double distance) const;

		/**
		*	Move the line to a specified point. 
		*
		*	@prarm point [in] The point.
		*
		*	@return A new line, obtained by moving this line 
		*	along the geodesic from origin to the point.
		*/
		Line2D moveTo(const Real2D& point) const;

		/**
		*	Move the origin.
		*	A positive distance moves the origin forward, a negative
		*	distance moves the origin backward. 
		*
		*	@param distance [in] (METERS) The distance.
		*
		*	@return The new point, obtaining by moving the origin along this line.
		*/
		Real2D pointFromOrigin(double distance) const;

		/**
		*	Rotate the line.
		*	A positive angle rotates in CLOCKWISE direction.
		*
		*	@param angle [in] (DECIMAL DEGREES) The angle.
		*
		*	@return The new line, obtained by rotating this line around
		*	its origin.
		*/
		Line2D rotate(double angle) const;

		/**
		*	Check if a specified point is on this line, using the
		*	global geometric tolerance defined for the current
		*	coordinate system.
		*
		*	@param point [in] The point to be tested.
		*
		*	@return True if point is on the line, flase otherwise.
		*/	
		bool contains(const Real2D& point) const;

		/**
		*	Determines on which side of the line the test point lies.
		*
		*	@param point [in] The point to be tested.
		*
		*	@return +1 if the point lies on the left of the line,
		*	-1 if the point lies on the right,
		*	0 if the point is on the line (using the geometric tolerance).
		*/
		int getPointSide(const Real2D& point) const;

		/**
		*	Get the intersection points between two lines.
		*
		*	@param line [in] The line to be intersected with this line.
		*
		*	@return The vector of intersection points.
		*/
		std::vector<Real2D> intersect(const Line2D& line) const;

		/**
		*	Compute if Line intersect Box2D.
		*
		*	@param box [in] The box to be intersected with this line.
		*
		*	@return True if line intersect Box, false otherwise.
		*/
		bool intersect(const Box& box) const;

		/**
		*	Given a point, return its projection on this line
		*
		*	@param point [in] The point to project.
		*
		*	@return The projected point.
		*/
		Real2D projectPoint(const Real2D& point) const;

		/** 
		*	Get the angle between this line and a target line.
		*
		*	The angle is positive if the source line (this) must rotate
		*	in CLOCKWISE direction to reach the target line,
		*	negative otherwise.
		*
		*	If the origins are different, the target line is translated
		*	to the source origin along the connecting geodesic and the
		*	angle is measured at the source line's origin.
		*
		*	@param target_line [in] The target line.
		*
		*	@return (DECIMAL DEGREES) A floating point number between -180 and 180.
		*/
		double angleBetween(const Line2D& target_line) const;

		///	Check if the point is on the positive ray semiplane.
		bool isForward(const Real2D& point) const;

		///	Check if the point is on the negative ray semiplane.
		bool isBackward(const Real2D& point) const;

	private:
		/**
		*  Contruct a line with the given origin and direction.
		*
		*	@param origin [in] The origin of the line.
		*	@param direction [in] The direction of the line. Its modulus can be
		*    different from 1, but must not be zero or an exception of type
		*    DegenerateGeometry will be thrown.
		*/
		// 		Line2D(const Real2D& origin,const Real2D& direction);

	private:
		Real2D m_origin;
		Real2D m_direction;
	};

	/**
	*	Constructs a line from origin and target point.
	*
	*	@param origin [in] The origin point.
	*	@param angle [in] The angle of line, aueronautical convention.
	*
	*	@return The new line.
	*/
	Line2D makeLine(const Real2D& origin, double angle);

	/**
	*	Constructs a line from origin and target point.
	*
	*	@param origin [in] The origin point.
	*	@param target [in] The target point.
	*
	*	@return The new line.
	*/
	Line2D makeLine(const Real2D& origin, const Real2D& target);

}

#endif // !NOSTOP_LINE2D

