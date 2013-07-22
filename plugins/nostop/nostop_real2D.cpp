// nostop_real2D.cpp: implementation of the Real2D class.
// 
/////////////////////////////////////////////////////// 

#include "nostop_real2D.h"

namespace NoStop
{
	// Returns 1 if the vector strictly belongs to the sector
	// made by vectors 1 and 2 (taken anti-clock-wise).
	// Returns 2 if the vector is superposed to vector 1 or 2.
	// Returns 0 otherwise.
	int Real2D::belongsToSector(const Real2D& vec1, const Real2D& vec2) const
	{
		Real2D vec0 = *this;
		REAL smod = this->sqrmod();
		// If the vector is superposed to vec1 returns 2.
		if((fabs(vec1^vec0)<smod*Math::TOLERANCE)&&(vec1*vec0>=0))
			return 2;
		// If the vector is superposed to vec2 returns 2.
		if((fabs(vec2^vec0)<smod*Math::TOLERANCE)&&(vec2*vec0>=0))
			return 2;
		// If vec1 is superposed to vec2 returns 1.
		if((fabs(vec1^vec2)<vec2.sqrmod()*Math::TOLERANCE)&&(vec1*vec2>=0))
			return 1;
		// Distinguishes between two cases:
		// a) vec1 and vec2 (taken anti-clock-wise) form an angle smaller than 180 degrees.
		if((vec1^vec2)>=0) {
			if(((vec1^vec0)>0)&&((vec0^vec2)>0))
				return 1;
			else
				return 0;
			// b) vec1 and vec2 (taken anti-clock-wise) form an angle greater than 180 degrees.
		} else {
			if(((vec1^vec0)>0)||((vec0^vec2)>0))
				return 1;
			else
				return 0;
		}
	}

	//////////////////////////////////////////////////////// 
	// Returns true if the vector belongs to triangle v1v2v3.
	// Returns false otherwise.
	bool Real2D::belongsToTriangle(const Real2D& v1,
		const Real2D& v2,
		const Real2D& v3,
		REAL tolerance) const
	{
		// Checks that the vector belongs to the triangle's bounding box.
		if((v[0]-tolerance > v1[0]) && (v[0]-tolerance > v2[0]) && (v[0]-tolerance > v3[0]))
			return false;
		if((v[0]+tolerance < v1[0]) && (v[0]+tolerance < v2[0]) && (v[0]+tolerance < v3[0]))
			return false;
		if((v[1]-tolerance > v1[1]) && (v[1]-tolerance > v2[1]) && (v[1]-tolerance > v3[1]))
			return false;
		if((v[1]+tolerance < v1[1]) && (v[1]+tolerance < v2[1]) && (v[1]+tolerance < v3[1]))
			return false;

		// Computes the normals to the triangles' edges.
		Real2D n1 = (v1-v2).normal().vers();
		Real2D n2 = (v2-v3).normal().vers();
		Real2D n3 = (v3-v1).normal().vers();

		// Checks that the vector belongs to the triangle
		// Modificato il 2/9/2009
		REAL s1 = (*this-v1)*n1;
		REAL check1 = (v3-v1)*n1;
		if (s1*check1 <= 0 && fabs(s1) > tolerance) return false;

		REAL s2 = (*this-v2)*n2;
		REAL check2 = (v1-v2)*n2;
		if (s2*check2 <= 0 && fabs(s2) > tolerance) return false;

		REAL s3 = (*this-v3)*n3;
		REAL check3 = (v2-v3)*n3;
		if (s3*check3 <= 0 && fabs(s3) > tolerance) return false;

		return true;
	}

	//////////////////////////////////////////////////////////////// 
	// Checks if the point belongs to the polygon of vertices "vert".
	bool Real2D::belongsToPolygon(const Real2D* vert, int nVert) const
	{
		// Corner cases
		if (nVert <= 2) return false;

		REAL x = this->v[0];
		REAL y = this->v[1];

		// Count the number of intersections of the polygon edges
		// with the half-line from this point (u,v) to (+inf,v)
		int count = 0;
		for (int i = nVert-1, j = 0; j < nVert; i = j++)
		{
			REAL yi = vert[i][1];
			REAL yj = vert[j][1];

			// If the edge crosses the horizontal line, check if the
			// intersection is on the right of the point.
			//
			// The intersection has coordinates
			//   x_int = ( xi*(y-yj) + xj*(yi-y) ) / (yi-yj)
			//   y_int = y
			// The test (x_int > x) has been split in two cases to avoid
			// division by (yi-yj), which can be very small.
			// 
			// Inequalities must be chosen carefully, because if there is an edge
			// lying on the half-line the number of intersections must be correct.
			if (yi < y && y <= yj)
			{
				// yi-yj < 0
				REAL t = vert[i][0]*(y-yj) + vert[j][0]*(yi-y);
				if (t < x*(yi-yj)) count++;
			}
			else if (yj < y && y <= yi)
			{
				// yi-yj > 0
				REAL t = vert[i][0]*(y-yj) + vert[j][0]*(yi-y);
				if (t > x*(yi-yj)) count++;
			}
		}

		return (count & 1) != 0;
	}

	/// Checks if the point belongs to the segment A-B.
	bool Real2D::belongsToSegment(const Real2D& A, const Real2D& B, double tolerance) const
	{
		// P means this
		Real2D AB = B - A;
		double ABsqrmod = AB.sqrmod();
		Real2D AP = *this - A;
		if( ABsqrmod < tolerance*tolerance )
		{
			if( AP < tolerance )
				return true;
			else
				return false;
		}

		double param = AB*AP;
		if( param < 0 || param > ABsqrmod )
			return false;

		Real2D proj = (param/ABsqrmod)*AB;
		if( (AP-proj).sqrmod() > tolerance*tolerance )
			return false;
		else
			return true;
	}

	/// Checks if the point belongs to the polyline points.
	bool Real2D::belongsToPolyline(const vector<Real2D>& points, double tolerance) const
	{
		if( points.size() < 2 )
			return false;

		size_t size = points.size()-1;
		for( size_t i = 0; i != size; i++)
		{
			if( this->belongsToSegment(points[i], points[i+1], tolerance) )
				return true;
		}
		return false;
	}

	//----------------
	// Static methods

	///////////////////////////////////////////////////////////////// 
	// Returns 1 if v1v2 intersects v3v4 (within the given tolerance).
	// Returns 2 if v1v2 is partially superposed to v3v4.
	// Returns 0 otherwise.
	int Real2D::checkIntersection(const Real2D& v1, const Real2D& v2,
		const Real2D& v3, const Real2D& v4)
	{
		REAL a,b,c,d,s,t;

		// Checks if the two bounding boxes intersect each other.
		// max(v1x,v2x)
		a = (v1.v[0]>v2.v[0]?v1.v[0]:v2.v[0]);
		// min(v3x,v4x)
		b = (v3.v[0]<v4.v[0]?v3.v[0]:v4.v[0]);
		if(a < b)
			return false;
		// max(v3x,v4x)
		a = (v3.v[0]>v4.v[0]?v3.v[0]:v4.v[0]);
		// min(v1x,v2x)
		b = (v1.v[0]<v2.v[0]?v1.v[0]:v2.v[0]);
		if(a < b)
			return false;
		// max(v1y,v2y)
		a = (v1.v[1]>v2.v[1]?v1.v[1]:v2.v[1]);
		// min(v3y,v4y)
		b = (v3.v[1]<v4.v[1]?v3.v[1]:v4.v[1]);
		if(a < b)
			return false;
		// max(v3y,v4y)
		a = (v3.v[1]>v4.v[1]?v3.v[1]:v4.v[1]);
		// min(v1y,v2y)
		b = (v1.v[1]<v2.v[1]?v1.v[1]:v2.v[1]);
		if(a < b)
			return false;

		// Searches for a solution of type:
		// 
		// v1 + s(v2 - v1) = v3 + t(v4 - v3)
		// 
		// with    0 <= s,t <= 1
		a = (v3-v1)^(v2-v1);
		b = (v3-v1)^(v4-v3);
		c = (v2-v1)^(v4-v3);
		d = (v2-v1).sqrmod();
		if(fabs(c) < d*Math::TOLERANCE) {
			// v1v2 and v3v4 are parallel.
			if(fabs(a) < d*Math::TOLERANCE) {
				// v1v2 and v3v4 are aligned.
				s = (v3-v1)*(v2-v1)/d;
				if((-Math::TOLERANCE < s)&&(s < 1. + Math::TOLERANCE))
					// v1v2 and v3v4 are partially superposed (v3 belongs to v1v2).
					return 2;
				s = (v4-v1)*(v2-v1)/d;
				if((-Math::TOLERANCE < s)&&(s < 1. + Math::TOLERANCE))
					// v1v2 and v3v4 are partially superposed (v4 belongs to v1v2).
					return 2;
				// v1v2 and v3v4 are aligned but not (even partially) superposed.
				return 0;
			} else {
				// v1v2 and v3v4 are parallel but not aligned.
				return 0;
			}
		}
		// v1v2 and v3v4 are not parallel.
		s = b/c;
		t = a/c;
		if((-Math::TOLERANCE < s)&&(s < 1. + Math::TOLERANCE)) {
			if((-Math::TOLERANCE < t)&&(t < 1. + Math::TOLERANCE)) {
				return 1;
			}
		}
		return 0;
	}

	///////////////////////////////////////////////////////////////////// 
	// Computes the intersection point, v, between segments v1v2 and v3v4.
	// Returns 1 if v1v2 intersects v3v4 (within the given tolerance).
	// Returns 2 if v1v2 is partially superposed to v3v4 (in this case the
	// intersection point is the midpoint of the intersection interval).
	// Returns 0 otherwise.
	int Real2D::computeIntersection(const Real2D& v1, const Real2D& v2,
		const Real2D& v3, const Real2D& v4,
		Real2D& v)
	{
		REAL a,b,c,d,s,t;

		// Checks if the two bounding boxes intersect each other.
		// max(v1x,v2x)
		a = (v1.v[0]>v2.v[0]?v1.v[0]:v2.v[0]);
		// min(v3x,v4x)
		b = (v3.v[0]<v4.v[0]?v3.v[0]:v4.v[0]);
		if(a < b)
			return false;
		// max(v3x,v4x)
		a = (v3.v[0]>v4.v[0]?v3.v[0]:v4.v[0]);
		// min(v1x,v2x)
		b = (v1.v[0]<v2.v[0]?v1.v[0]:v2.v[0]);
		if(a < b)
			return false;
		// max(v1y,v2y)
		a = (v1.v[1]>v2.v[1]?v1.v[1]:v2.v[1]);
		// min(v3y,v4y)
		b = (v3.v[1]<v4.v[1]?v3.v[1]:v4.v[1]);
		if(a < b)
			return false;
		// max(v3y,v4y)
		a = (v3.v[1]>v4.v[1]?v3.v[1]:v4.v[1]);
		// min(v1y,v2y)
		b = (v1.v[1]<v2.v[1]?v1.v[1]:v2.v[1]);
		if(a < b)
			return false;

		// Searches for a solution of type:
		// 
		// v1 + s(v2 - v1) = v3 + t(v4 - v3)
		// 
		// with    0 <= s,t <= 1
		a = (v3-v1)^(v2-v1);
		b = (v3-v1)^(v4-v3);
		c = (v2-v1)^(v4-v3);
		d = (v2-v1).sqrmod();
		if(fabs(c) <= d*Math::TOLERANCE) {
			// v1v2 and v3v4 are parallel.
			if(fabs(a) < d*Math::TOLERANCE) {
				// v1v2 and v3v4 are aligned.
				if(d < Math::TOLERANCE2)
					s = 0.5;
				else
					s = (v3-v1)*(v2-v1)/d;
				if((-Math::TOLERANCE < s)&&(s < 1. + Math::TOLERANCE)) {
					// v1v2 and v3v4 are partially superposed (v3 belongs to v1v2).
					if((v4-v3)*(v2-v1) > 0)
						// v2 belongs to v3v4.
						v = 0.5*(v2+v3);
					else
						// v1 belongs to v3v4.
						v = 0.5*(v1+v3);
					return 2;
				}
				if(d < Math::TOLERANCE2)
					s = 0.5;
				else
					s = (v4-v1)*(v2-v1)/d;
				if((-Math::TOLERANCE < s)&&(s < 1. + Math::TOLERANCE)) {
					// v1v2 and v3v4 are partially superposed (v4 belongs to v1v2).
					if((v4-v3)*(v2-v1) > 0)
						// v1 belongs to v3v4.
						v = 0.5*(v1+v4);
					else
						// v2 belongs to v3v4.
						v = 0.5*(v2+v4);
					return 2;
				}
				// v1v2 and v3v4 are aligned but not (even partially) superposed.
				return 0;
			} else {
				// v1v2 and v3v4 are parallel but not aligned.
				return 0;
			}
		}
		// v1v2 and v3v4 are not parallel.
		s = b/c;
		t = a/c;
		if((-Math::TOLERANCE < s)&&(s < 1. + Math::TOLERANCE)) {
			if((-Math::TOLERANCE < t)&&(t < 1. + Math::TOLERANCE)) {
				v = v1 + s*(v2 - v1);
				return 1;
			}
		}
		return 0;
	}

	/////////////////////////////////////////////////////////////////// 
	// Computes the barycenter of a polygon.
	Real2D Real2D::barycenter(const vector<Real2D>& points)
	{
		Real2D center(0,0);
		REAL weight = 0;
		for (size_t i = 0, j = points.size()-1; i < points.size(); j = i++)
		{
			REAL w = (points[i]-points[j]) ^ points[i];
			center += w/3 * (points[i] + points[j]);
			weight += w;
		}
		if (fabs(weight) > Math::TOLERANCE2)
			center /= weight;
		return center;
	}
}
