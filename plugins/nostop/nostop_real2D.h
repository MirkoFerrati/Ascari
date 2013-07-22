/// nostop_Real2D.h: interface for the Real2D class.
/// 
////////////////////////////////////////////////// 

#ifndef NOSTOP_REAL2D_H
#define NOSTOP_REAL2D_H
#pragma once

#include "nostop_math.h"

#include <vector>
#include <cassert>
#include <cfloat>

namespace NoStop
{
	/// Class implementing a vector in real 2D space.
	class Real2D  
	{
	public:
		/// components
		REAL v[2];

	public:
		/// Constructor
		Real2D()
		{
			this->v[0]= 0;
			this->v[1]= 0;
		}

		/// Constructor from real components.
		Real2D(REAL xx, REAL yy)
		{
			this->v[0]= xx;
			this->v[1]= yy;
		}

		/// Check for NaN
		bool isnan() const
		{
			return std::isnan(v[0]) || std::isnan(v[1]);
		}

		/// Check for NaN or infinite
		bool isfinite() const
		{
			return std::isfinite(v[0]) && std::isfinite(v[1]);
		}

		/// Smaller than operator. Returns true if the maximum (absolute)
		/// component of the vector is smaller than second.
		bool operator<(REAL second) const
		{
			if(fabs(this->v[0]) >= (REAL)second)
				return false;
			if(fabs(this->v[1]) >= (REAL)second)
				return false;
			if(v[0]*v[0] + v[1]*v[1] < (REAL)second*second)
				return true;
			return false;
		}

		/// Greater than operator. Returns true if at least one (absolute)
		/// component of the vector is greater than second.
		bool operator>(REAL second) const
		{
			if(fabs(this->v[0]) > (REAL)second)
				return true;
			if(fabs(this->v[1]) > (REAL)second)
				return true;
			if(v[0]*v[0] + v[1]*v[1] <= (REAL)second*second)
				return false;
			return true;
		}

		/// Update the vector taking the minimun coordinates between itself and second
		void minimun(const Real2D& second)
		{
			for(int i =0; i < 2; i++) {
				if (this->v[i] > second.v[i])
					this->v[i] = second.v[i];
			}
		}

		/// Update the vector taking the maximum coordinates between itself and second
		void maximum(const Real2D& second)
		{
			for(int i =0; i < 2; i++) {
				if (this->v[i] < second.v[i])
					this->v[i] = second.v[i];
			}
		}

		/// Normal of the vector (rotated vector 90 degrees anti-clock-wise).
		Real2D normal() const
		{
			return Real2D(-this->v[1],this->v[0]);
		}

		/// Vector rotated theta radians (the rotation is anti-clock-wise if theta is positive).
		Real2D rotateRad(const double theta) const
		{
			double sinTheta = sin(theta);
			double cosTheta = cos(theta);
			return Real2D(cosTheta*this->v[0] - sinTheta*this->v[1], sinTheta*this->v[0] + cosTheta*this->v[1]);
		}

		/// Maximum (absolute) component of the vector.
		REAL maxcomp() const
		{
			REAL ax = (REAL)fabs(v[0]);
			REAL ay = (REAL)fabs(v[1]);
			return (ax>ay?ax:ay);
		}

		/// Checks if the two points coincide within the given tolerance.
		bool equals(const Real2D& n, REAL tolerance) const
		{
			if(fabs(this->v[0] - n.v[0]) > tolerance) return false;
			if(fabs(this->v[1] - n.v[1]) > tolerance) return false;
			if((this->v[0]-n.v[0])*(this->v[0]-n.v[0]) + (this->v[1]-n.v[1])*(this->v[1]-n.v[1]) < tolerance*tolerance)
				return true;
			return false;
		}	 

		/// Cross product of two vectors
		REAL operator^(const Real2D& second) const
		{
			return (v[0]*second.v[1] - v[1]*second.v[0]);
		}

		/// Modulus of the vector
		REAL mod() const
		{
			double sqmod = (double)(v[0]*v[0] + v[1]*v[1]);
			return (REAL)sqrt(sqmod);
		}

		/// Squared modulus of the vector.
		REAL sqrmod() const
		{
			return (REAL)(v[0]*v[0] + v[1]*v[1]);
		}

		/// Versor of the vector.
		Real2D vers() const
		{
			REAL modulus = mod();
			Real2D vers;
			if(modulus > Math::TOLERANCE2) {
				modulus = 1.0 / modulus;
				vers.v[0] = v[0] * modulus;
				vers.v[1] = v[1] * modulus;
			}
			return vers;
		}

		/// Returns 1 if the vector strictly belongs to the sector
		/// made by vectors 1 and 2 (taken anti-clock-wise).
		/// Returns 2 if the vector is superposed to vector 1 or 2.
		/// Returns 0 otherwise.
		int belongsToSector(const Real2D& vec1, const Real2D& vec2) const;


		/// Returns true if the point belongs to triangle v1v2v3.
		/// Returns false otherwise.
		bool belongsToTriangle(const Real2D& v1,
			const Real2D& v2,
			const Real2D& v3,
			REAL tolerance = Math::TOLERANCE) const;

		/// Checks if the point belongs to the polygon of vertices "vert".
		bool belongsToPolygon(const Real2D* vert, int nVert) const;

		/// Checks if the point belongs to the polygon of vertices "vert".
		bool belongsToPolygon(const vector<Real2D>& vert) const
		{
			return (vert.size() > 0) && this->belongsToPolygon(&vert[0], (int)vert.size());
		}

		/// Checks if the point belongs to the polygon of vertices "vert".
		template<int nVert> bool belongsToPolygon(const Real2D (&vert)[nVert]) const
		{
			return this->belongsToPolygon(&vert[0], nVert);
		}

		/// Checks if the point belongs to the segment A-B.
		bool belongsToSegment(const Real2D& A, const Real2D& B, double tolerance) const;

		/// Checks if the point belongs to the polyline points.
		bool belongsToPolyline(const vector<Real2D>& points, double tolerance) const;

		//------------------
		// Inline operators

		/// Equality operator
		bool operator==(const Real2D& second) const
		{
			if( v[0] != second.v[0] ) return false;
			if( v[1] != second.v[1] ) return false;
			return true;
		}

		/// Inequality operator
		bool operator!=(const Real2D& second) const
		{
			return !( *this == second );
		}

		/// Operator[]
		REAL operator[](int i) const
		{
			assert(i>=0 && i<2);
			return v[i];
		}

		/// Operator[]
		REAL& operator[](int i)
		{
			assert(i>=0 && i<2);
			return this->v[i];
		}

		/// Operator()
		REAL operator()(int i) const
		{
			assert(i>=0 && i<2);
			return this->v[i];
		}

		/// Operator()
		REAL& operator()(int i)
		{
			assert(i>=0 && i<2);
			return this->v[i];
		}

		/// Addition of a real vector.
		Real2D & operator+=(const Real2D& second)
		{
			this->v[0] += second.v[0];
			this->v[1] += second.v[1];
			return (*this);
		}

		/// Subtraction of a real vector.
		Real2D & operator-=(const Real2D& second)
		{
			this->v[0] -= second.v[0];
			this->v[1] -= second.v[1];
			return (*this);
		}

		/// Product times a real double number.
		Real2D & operator*=(REAL second)
		{
			this->v[0] *= (REAL)second;
			this->v[1] *= (REAL)second;
			return (*this);
		}

		/// Division by a real double number.
		Real2D & operator/=(REAL second)
		{
			REAL invsecond = (REAL)1.0 / second;
			this->v[0] *= invsecond;
			this->v[1] *= invsecond;
			return (*this);
		}

		/// Scalar product of two vectors
		REAL operator*(const Real2D& second) const
		{
			return (v[0]*second.v[0] + v[1]*second.v[1]);
		}

		/// Sum of two vectors
		Real2D operator+(const Real2D& second) const
		{
			return Real2D( v[0]+second.v[0], v[1]+second.v[1] );
		}

		/// Difference of two vectors
		Real2D operator-(const Real2D& second) const
		{
			return Real2D( v[0]-second.v[0], v[1]-second.v[1] );
		}

		/// Opposite of a vector
		Real2D operator-() const
		{
			return Real2D( -v[0], -v[1] );
		}

		/// Multiplication of a vector times a real float scalar
		Real2D operator*(REAL second) const
		{
			return Real2D( v[0]*second, v[1]*second );
		}

		/// Division of a vector by a real float scalar
		Real2D operator/(REAL second) const
		{
			REAL invsecond = (REAL)1. / second;
			return Real2D(v[0]*invsecond, v[1]*invsecond);
		}

		//----------------
		// Static methods

		/// Returns 1 if v1v2 intersects v3v4 (within the given tolerance).
		/// Returns 2 if v1v2 is partially superposed to v3v4.
		/// Returns 0 otherwise.
		static int checkIntersection(const Real2D& v1, const Real2D& v2,
			const Real2D& v3, const Real2D& v4);

		/// Computes the intersection point, v, between segments v1v2 and v3v4.
		/// Returns 1 if v1v2 intersects v3v4 (within the given tolerance).
		/// Returns 2 if v1v2 is partially superposed to v3v4 (in this case the
		/// intersection point is the midpoint of the intersection interval).
		/// Returns 0 otherwise.
		static int computeIntersection(const Real2D& v1, const Real2D& v2,
			const Real2D& v3, const Real2D& v4,
			Real2D& v);

		/// Computes the barycenter of a polygon.
		static Real2D barycenter(const vector<Real2D>& points);
	};

	/// Multiplication of a real scalar times a vector
	inline Real2D operator*(REAL first, const Real2D& second)
	{
		return second * first;
	}
}

#endif // Real2D_H
