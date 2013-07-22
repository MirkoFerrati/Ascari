/// nostop_math.h: interface for the Math class.
/// 
//////////////////////////////////// 

#ifndef NOSTOP_MATH_H
#define NOSTOP_MATH_H
#pragma once

#include <cstdlib>

#ifdef _WIN32
#include <crtdbg.h>
#include <cfloat>
namespace std {
	inline int isnan(double d) { return _isnan(d); }
	inline int isfinite(double d) { return _finite(d); }
}
#endif

#include <vector>
#include <list>
#include <cmath>

using std::vector;
//using std::isnan;
using std::isfinite;

namespace NoStop
{
	//////////////////////////////////// 
	/// Real type
	//////////////////////////////////// 
	typedef double REAL;

	//////////////////////////////////// 
	/// Mathematical constants
	//////////////////////////////////// 
	class Real2D;

	/// Class implementing several mathematical functions/utilities
	class Math
	{
	public:
		static const REAL TOLERANCE;
		static const REAL TOLERANCE2;
		/// Maximum number of steps for iterative numerical algorithms.
		static const int MAX_NUM_STEPS;

		/// Mathematical constants
		static const REAL E;
		static const REAL Log2_E;
		static const REAL Log10_E;
		static const REAL Ln2;
		static const REAL Ln10;
		static const REAL Pi;
		static const REAL TwoPi;
		static const REAL PiDiv2;
		static const REAL PiDiv4;
		static const REAL InvPi;
		static const REAL TwoDivPi;
		static const REAL Sqrt2;
		static const REAL InvSqrt2;

		static const REAL LargeReal;    // Largest finite REAL number
		static const REAL Infinity;     // Positive infinity
		static const REAL Epsilon;      // Smallest positive number x such that 1+x!=1

	public:
		/// Constructor.
		Math();

		/// Destructor.
		virtual ~Math();

		/// Angle to Radiants
		static REAL toRad(REAL x)
		{
			return x * (Math::Pi / 180.0);
		}

		/// Angle to degrees
		static REAL toDeg(REAL x)
		{
			return x * (180.0 / Math::Pi);
		}


		/// Computes the polar angular coordinate of a cartesian point
		/// in the 2D real plane.
		/// The angle is measured in radians, in counterclockwise direction
		/// from the x axis, and the range is [0,2Pi).
		static double polarPhi2D(REAL x,REAL y);
		static double polarPhi2D(const Real2D& v);

		/// Computes the signed area of the polygon of vertices "vert".
		/// The area is positive if the points are taken anti-clock-wise
		static REAL polygonArea(const vector<Real2D>& vert);

		/// Tessellates by ear-cutting a 2D planar polygon identified
		/// by its vertices. Returns a vector of points identifying the
		/// vertices of the triangles of the tessellation.
		static vector<Real2D> polygonEarCut(const vector<Real2D>& vert);

		/// Cross product of the vectors (x1,y1) e (x2,y2)
		static REAL cross(REAL x1, REAL y1, REAL x2, REAL y2)
		{
			return (x1*y2-x2*y1);
		}

		/// Computes the factorial of the integer n.
		static int factorial(int n);

		/// Truncates the given REAL number to the required precision
		/// (number of digits).
		/// The parameter "type" gives the type of approximation:
		///   0: "round"
		///   1: "ceil"
		///  -1: "floor"
		static REAL truncate(REAL x, int nDigits, int type);

		/// Returns the sign of the REAL number a.
		static int sign(REAL a);

		/// Sorts an array of integers into ascending numerical order, by straight insertion
		/// NB: this is an N^2 order algorithm. Do not use it for large arrays.
		static void sort(vector<int>& vec);

		/// Sorts an array of REALs into ascending numerical order, by straight insertion
		/// NB: this is an N^2 order algorithm. Do not use it for large arrays.
		static void sort(vector<REAL>& vec);

		/// Sorts an array of REALs into ascending numerical order and
		/// returns the corresponding permutation vector.
		/// NB: this is an N^2 order algorithm. Do not use it for large arrays.
		static vector<int> sortWithPermutation(vector<REAL>& vec);

		/// Computes the intersection of the two interval vectors.
		static vector<REAL> intervalsIntersection(vector<REAL> const& i1, vector<REAL> const& i2);

		/// Computes the root(s) of the quadratic equation
		///  a x^2 + b x + c = 0
		/// with real coefficients a,b,c. Returns the number of (real) solutions.
		static int quadraticRoots(const REAL& a, const REAL& b, const REAL& c, REAL& x1, REAL&x2);

		/// Checks if the triangle and a segment are (partially) overlapped.
		/// The overlapping segment is saved in "intersection".
		/// Returns the number of points saved in "intersection".
		static int triangleSegmentOverlap(Real2D tri[3], Real2D seg[2], Real2D intersection[2]);

		/// Computes the intersection of two convex polygons.
		/// Returns the number of vertices of the intersection polygon.
		static int computeConvexPolygonsIntersection(Real2D* p, int np,
			Real2D* q, int nq,
			Real2D* intersection);

		/// Computes the convex hull of the data points stored in "points",
		/// points is a 2*size double stream
		static int compute2DConvexHull(int size, REAL *points, Real2D*& hull, double *scale);

		/// Extrudes the convex hull surface to a distance leng wih aperture alpha
		static int extrudeConvexHullAlongZ(int size, Real2D* hull,
			REAL leng, REAL alpha,
			Real2D*& extrudedHull);

		/// Check box intersection in 2D
		static bool checkBoxIntersection(
			Real2D& aMin, Real2D& aMax,
			Real2D& bMin, Real2D& bMax);

		/// Compute normals of regular mesh formed by quadrangles
		/// vert[i+j*nV] is the vertex(i,j)
		static void computeNormalsOfRegularMesh(
			int nU, int nV, vector<float> &vert,
			vector<float> &normal);

		/// Compute normals of regular mesh formed by quadrangles
		/// vert[i+j*nV] is the vertex(i,j)
		static void computeNormalsOfRegularMesh(
			int nU, int nV, vector<double> &vert,
			vector<double> &normal);

		/// Returns the distance between the point p and the segment ab (2D)
		/// If q is not NULL, the point of ab nearest to p is stored in the memory pointed to by q.
		static REAL distancePointSegment(const Real2D &p, const Real2D &a, const Real2D &b, Real2D *q);

		/// Returns the distance between the segment ab and the segment cd (2D)
		/// If p is not NULL, the point of ab nearest to cd is stored in the memory pointed to by p.
		/// If q is not NULL, the point of cd nearest to ab is stored in the memory pointed to by q.
		static REAL distanceSegmentSegment(const Real2D &a, const Real2D &b, const Real2D &c, const Real2D &d, Real2D *p, Real2D *q);

		/// Returns the angle between two vectors in 2D space.
		/// The angle is measured in radians (moving in counterclockwise direction
		/// from 'v' to 'w') and is between -Pi and Pi.
		static REAL angleBetween(const Real2D &v, const Real2D& w);

		// Convert dd:mm:ss in decimal degrees
		static bool dmsToDegrees(int dd, int mm, double ss, double &deg)
		{
			if(mm<0 || mm>=60 || ss<-TOLERANCE || ss>60.+TOLERANCE) return false;
			deg= dd+(mm+ss/60.)/60.;
			return true;
		}

		// Convert dd:mm:ss in decimal degrees
		static bool degreesToDms(double deg, int &dd, int &mm, double &ss)
		{
			dd= int(deg);
			mm= int((deg-dd)*60.);
			ss= deg-(dd+mm/60.);
			return true;
		}

		/// Corrected inverse trigonometric functions

		/// Cos: argument is in Radians
		static double cosR(double x)
		{
			return cos(x);
		}

		/// Sin: argument is in Radians
		static double sinR(double x)
		{
			return sin(x);
		}

		// Tan: argument is in Radians
		static double tanR(double x)
		{
			return tan(x);
		}

		/// Cos: argument is in Degrees
		static double cosD(double x)
		{
			return cos(toRad(x));
		}

		/// Sin: argument is in Degrees
		static double sinD(double x)
		{
			return sin(toRad(x));
		}

		// Tan: argument is in Degrees
		static double tanD(double x)
		{
			return tan(toRad(x));
		}

		/// Acos: return angle in Radians
		static double acosR(double c)
		{
			if( c >= 1. )
				return 0.;
			else if( c <= -1. )
				return Math::Pi;
			else
				return acos(c);
		}

		/// Asin: return angle in Radians
		static double asinR(double s)
		{
			if( s >= 1. )
				return Math::PiDiv2;
			else if( s <= -1. )
				return -Math::PiDiv2;
			else
				return asin(s);
		}

		// Atan: return angle in Radians
		static double atanR(double t)
		{
			return atan(t);
		}

		/// Acos: return angle in Degrees
		static double acosD(double c)
		{
			if( c >= 1. )
				return 0.;
			else if( c <= -1. )
				return 180.;
			else
				return toDeg(acos(c));
		}

		/// Asin: return angle in Degrees
		static double asinD(double s)
		{
			if( s >= 1. )
				return 90.;
			else if( s <= -1. )
				return -90.;
			else
				return toDeg(asin(s));
		}

		// Atan: return angle in Degrees
		static double atanD(double t)
		{
			return toDeg(atan(t));
		}

		// Unit conversion ( meters to ...)
		static double convert_m_to_mm(double m)
		{
			return m*1000;
		}

		static double convert_m_to_cm(double m)
		{
			return m*100;
		}

		static double convert_m_to_km(double m)
		{
			return m*0.001;
		}

		static double convert_m_to_mil(double m)
		{
			return m*39370.07874;
		}

		static double convert_m_to_in(double m)
		{
			return m*39.37007874;
		}

		static double convert_m_to_ft(double m)
		{
			return m*3.280839895;
		}

		static double convert_m_to_yd(double m)
		{
			return m*1.093613298;
		}

		static double convert_m_to_nm(double m)
		{
			return m*5.39956803456e-4;
		}

		// Unit conversion ( ... to meters )
		static double convert_mm_to_m(double mm)
		{
			return mm*0.001;
		}

		static double convert_cm_to_m(double cm)
		{
			return cm*0.01;
		}

		static double convert_km_to_m(double km)
		{
			return km*1000;
		}

		static double convert_mil_to_m(double mil)
		{
			return mil*0.0000254;
		}

		static double convert_in_to_m(double in)
		{
			return in*0.0254;
		}

		static double convert_ft_to_m(double ft)
		{
			return ft*0.3048;
		}

		static double convert_yd_to_m(double yd)
		{
			return yd*0.9144;
		}

		static double convert_nm_to_m(double nm)
		{
			return nm*1852;
		}

		//For plotting
		static void ComputeDistributionValues(const vector<float>& data,
			REAL minVal,
			REAL maxVal,
			vector<float>& x,
			vector<float>& y,
			int nPoints = 0);

		/// True if test point is inside the vertex chain
		static bool polygonContains(std::vector<Real2D> const& vertex, Real2D const& test);

	};
}


#endif // NOSTOP_MATH_H
