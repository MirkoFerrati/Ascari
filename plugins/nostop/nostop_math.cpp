/// Math.cpp: implementation of the Math class.
/// 
//////////////////////////////////// 

#include "nostop_math.h"

#include "nostop_real2D.h"
#include "nostop_box.hpp"

#include <list>
#include <algorithm>
using std::vector; 
using std::list;

#include <cfloat>
#include <limits>

namespace NoStop
{
	/// Static constants definition.
	const REAL Math::TOLERANCE = 1.e-8;
	const REAL Math::TOLERANCE2 = 1.e-16;
	const int Math::MAX_NUM_STEPS = 20;

	const REAL Math::E=			2.71828182845904523536;
	const REAL Math::Log2_E=		1.44269504088896340736;
	const REAL Math::Log10_E=	0.434294481903251827651;
	const REAL Math::Ln2=		0.693147180559945309417;
	const REAL Math::Ln10=		2.30258509299404568402;
	const REAL Math::Pi=			3.14159265358979323846;
	const REAL Math::TwoPi=		6.283185307179586476925;
	const REAL Math::PiDiv2=		1.57079632679489661923;
	const REAL Math::PiDiv4=		0.785398163397448309616;
	const REAL Math::InvPi=		0.318309886183790671538;
	const REAL Math::TwoDivPi=	0.636619772367581343076;
	const REAL Math::Sqrt2=		1.41421356237309504880;
	const REAL Math::InvSqrt2=	0.707106781186547524401;

	const REAL Math::LargeReal=  std::numeric_limits<REAL>::max();
	const REAL Math::Infinity=   std::numeric_limits<REAL>::infinity();
	const REAL Math::Epsilon=    std::numeric_limits<REAL>::epsilon();

	Math mathInitializer;

	/// Constructor.
	Math::Math()
	{}

	/// Destructor.
	Math::~Math()
	{}

	//////////////////////////////////// 
	// Computes the polar angular coordinate of a cartesian point
	// in the 2D real plane.
	// The angle is measured in radians, in counterclockwise direction
	// from the x axis, and the range is [0,2Pi).
	//////////////////////////////////// 

	double Math::polarPhi2D(REAL x, REAL y)
	{
		//The atan2 function does exactly what we need, but it returns
		//the result in the (-Pi,Pi] range. We just have to correct the output.
		double phi = atan2(y, x);
		if (phi < 0) phi += Math::TwoPi;
		return phi;
	}

	double Math::polarPhi2D(const Real2D& v)
	{
		return Math::polarPhi2D(v[0],v[1]);
	}

	//////////////////////////////////// 
	/// Computes the signed area of the polygon of vertices "vert".
	/// The area is positive if the points are taken anti-clock-wise
	//////////////////////////////////// 

	REAL Math::polygonArea(const vector<Real2D>& vert)
	{
		REAL area;
		int i,n;

		area = 0;
		n = vert.size();
		for(i=0; i<n; i++)
			area += vert[i]^vert[(i+1)%n];
		return (REAL) (area/2.);
	}

	//////////////////////////////////////////////////////////////////// 
	/// Disentangles segments v1v2 and v3v4 if they intersect each other.
	/// Returns 1 if v1v2 intersects v3v4 (within the given tolerance).
	/// Returns 2 if v1v2 is partially superposed to v3v4.
	/// Returns 0 otherwise.
	int disentangleSegments(Real2D& v1, Real2D& v2,
		Real2D& v3, Real2D& v4)
	{
		REAL a,b,c,d,s,t;

		/// Checks if the two bounding boxes intersect each other.
		/// max(v1x,v2x)
		a = (v1.v[0]>v2.v[0]?v1.v[0]:v2.v[0]);
		/// min(v3x,v4x)
		b = (v3.v[0]<v4.v[0]?v3.v[0]:v4.v[0]);
		if(a < b)
			return 0;
		/// max(v3x,v4x)
		a = (v3.v[0]>v4.v[0]?v3.v[0]:v4.v[0]);
		/// min(v1x,v2x)
		b = (v1.v[0]<v2.v[0]?v1.v[0]:v2.v[0]);
		if(a < b)
			return 0;
		/// max(v1y,v2y)
		a = (v1.v[1]>v2.v[1]?v1.v[1]:v2.v[1]);
		/// min(v3y,v4y)
		b = (v3.v[1]<v4.v[1]?v3.v[1]:v4.v[1]);
		if(a < b)
			return 0;
		/// max(v3y,v4y)
		a = (v3.v[1]>v4.v[1]?v3.v[1]:v4.v[1]);
		/// min(v1y,v2y)
		b = (v1.v[1]<v2.v[1]?v1.v[1]:v2.v[1]);
		if(a < b)
			return 0;

		/// Searches for a solution of type:
		/// 
		/// v1 + s(v2 - v1) = v3 + t(v4 - v3)
		/// 
		/// with    0 <= s,t <= 1
		a = (v3-v1)^(v2-v1);
		b = (v3-v1)^(v4-v3);
		c = (v2-v1)^(v4-v3);
		d = (v2-v1).sqrmod();
		if(fabs(c) <= d*Math::TOLERANCE) {
			/// v1v2 and v3v4 are parallel.
			if(fabs(a) < d*Math::TOLERANCE) {
				/// v1v2 and v3v4 are aligned.
				if(d < Math::TOLERANCE2)
					s = 0.5;
				else
					s = (v3-v1)*(v2-v1)/d;
				if((Math::TOLERANCE < s)&&(s < 1. - Math::TOLERANCE)) {
					/// v1v2 and v3v4 are partially superposed (v3 belongs to v1v2).
					if((v4-v3)*(v2-v1) > 0) {
						/// v2 belongs to v3v4.
						v2 = 0.5*(v2+v3);
						v3 = v2;
					} else {
						/// v1 belongs to v3v4.
						v1 = 0.5*(v1+v3);
						v3 = v1;
					}
					return 2;
				}
				if(d < Math::TOLERANCE2)
					s = 0.5;
				else
					s = (v4-v1)*(v2-v1)/d;
				if((Math::TOLERANCE < s)&&(s < 1. - Math::TOLERANCE)) {
					/// v1v2 and v3v4 are partially superposed (v4 belongs to v1v2).
					if((v4-v3)*(v2-v1) > 0) {
						/// v1 belongs to v3v4.
						v1 = 0.5*(v1+v4);
						v4 = v1;
					} else {
						/// v2 belongs to v3v4.
						v2 = 0.5*(v2+v4);
						v4 = v2;
					}
					return 2;
				}
				/// v1v2 and v3v4 are aligned but not (even partially) superposed.
				return 0;
			} else {
				/// v1v2 and v3v4 are parallel but not aligned.
				return 0;
			}
		}
		/// v1v2 and v3v4 are not parallel.
		s = b/c;
		t = a/c;
		if((Math::TOLERANCE < s)&&(s < 1. - Math::TOLERANCE)) {
			if((Math::TOLERANCE < t)&&(t < 1. - Math::TOLERANCE)) {
				if(s > 0.5)
					v2 = v1 + s*(v2 - v1);
				else
					v1 = v1 + s*(v2 - v1);
				if(t > 0.5)
					v4 = v3 + t*(v4 - v3);
				else
					v3 = v3 + t*(v4 - v3);
				return 1;
			}
		}
		return 0;
	}

	vector<Real2D> Math::polygonEarCut(const vector<Real2D>& vertIn)
	{
		if (vertIn.empty()) return vector<Real2D>();

		// Computes the initial vector of vertices of the polygon
		// eliminating repeated ones and reversing its order if
		// the polygon is walked clockwise.
		REAL area = Math::polygonArea(vertIn);
		if (fabs(area) < 100*Math::TOLERANCE2)
			return vector<Real2D>();

		list<Real2D> vertList;
		vertList.push_back(vertIn.front());
		if(area > 0)
		{
			for (size_t i = 1; i != vertIn.size(); i++)
				if (! vertIn[i].equals(vertList.back(),Math::TOLERANCE))
					vertList.push_back(vertIn[i]);
		}
		else
		{
			for (size_t i = vertIn.size()-1; i != 0; i--)
				if (! vertIn[i].equals(vertList.back(),Math::TOLERANCE))
					vertList.push_back(vertIn[i]);
		}

		if (vertList.size() < 3)
			return vector<Real2D>();

		// Eliminates self-intersections of the polygon's segments.
		// Spin (seg1a,seg1b) across all the segments
		list<Real2D>::iterator seg1a = -- vertList.end();
		list<Real2D>::iterator seg1b = vertList.begin();
		for (; seg1b != vertList.end(); seg1a = seg1b++)
		{
			// Spin (seg2a,seg2b) across all the segments except those adjacent to (seg1a,seg1b)
			list<Real2D>::iterator seg2a = seg1b; ++seg2a;
			if (seg2a == vertList.end() || seg2a == seg1a) continue;
			list<Real2D>::iterator seg2b = seg2a; ++seg2b;
			for (; seg2b != vertList.end() && seg2b != seg1a; seg2a = seg2b++)
			{
				disentangleSegments(*seg1a,*seg1b,*seg2a,*seg2b);
			}
		}

#ifdef _PRINT_NODE
		printNode("C:/Users/s.nardi/Desktop/temp/eTod/Error_Obstalce/PTS/meta.py", vertList);
#endif

		// Computes the tessellation of the polygon.
		// Cycles over all the vertices looking for an ear, until
		// the loop is given by a single triangle or a needle.
		vector<Real2D> vertOut;
		size_t skip_count = 0;
		while (vertList.size() > 2)
		{
			// Notice: there are cases where the algorithm can't find a good triangulation.
			// This usually comes from self-intersecting loops that the disentangleSegments
			// function above can't handle properly.
			// In this case, usually the best solution is to ignore the problem and return
			// a partial triangulation. The following test enables this back-door.
			if (skip_count++ > vertList.size()) break;

			list<Real2D>::iterator pa = vertList.begin();
			list<Real2D>::iterator pb = pa; ++pb;
			list<Real2D>::iterator pc = pb; ++pc;

			// Remove degenerate sides
			if (pb->equals(*pa,Math::TOLERANCE) ||
				pb->equals(*pc,Math::TOLERANCE))
			{
				vertList.erase(pb);
				skip_count = 0;
				continue;
			}

			// If the current "would-be-ear" is actually a needle removes it from the vertex chain.
			if (pa->equals(*pc,Math::TOLERANCE))
			{
				vertList.erase(pb);
				vertList.erase(pc);
				skip_count = 0;
				continue;
			}

			// Computes the sine of the internal angle of the current vertex.
			REAL theta = (*pc - *pb).vers() ^ (*pa - *pb).vers();

			// If theta is negative skips the current vertex.
			if (theta < -Math::TOLERANCE)
			{
				vertList.splice(vertList.end(),vertList,pa);
				continue;
			}

			// If the angle is flat removes the current vertex from the chain.
			if (theta < Math::TOLERANCE)
			{
				vertList.erase(pb);
				skip_count = 0;
				continue;
			}

			// Checks that no vertex is contained by the current "would-be-ear".
			bool must_skip = false;
			for (list<Real2D>::iterator pd = pc; ++pd != vertList.end(); )
			{
				if (pd->belongsToTriangle(*pb,*pc,*pa))
				{
					//	Nardi: 2012_07_16
					if( !pd->equals(*pb, Math::TOLERANCE) &&
						!pd->equals(*pa, Math::TOLERANCE) &&
						!pd->equals(*pc, Math::TOLERANCE) )
					{
						must_skip = true;
						break;
					}
				}
			}
			if (must_skip)
			{
				vertList.splice(vertList.end(),vertList,pa);
				continue;
			}

			// Adds the current ear to the triangulation and removes its tip
			// from the vertex chain.
			vertOut.push_back(*pa);
			vertOut.push_back(*pb);
			vertOut.push_back(*pc);

			vertList.erase(pb);
			vertList.splice(vertList.end(),vertList,pa);
			skip_count = 0;
		}

		return vertOut;
	}

	//////////////////////////////////// 
	/// Sorts an array of integer into ascending numerical order, by straight insertion
	/// Edit by Saccavino (28/10/08): use the STL's sort algorithm
	//////////////////////////////////// 

	void Math::sort(vector<int>& vec)
	{
		std::sort(vec.begin(), vec.end());
	}

	//////////////////////////////////// 
	/// Sorts an array of REALs into ascending numerical order, by straight insertion
	/// Edit by Saccavino (28/10/08): use the STL's sort algorithm
	//////////////////////////////////// 

	void Math::sort(vector<REAL>& vec)
	{
		std::sort(vec.begin(), vec.end());
	}

	//////////////////////////////////// 
	/// Sorts an array of REALs into ascending numerical order and
	/// returns the corresponding permutation vector.
	/// Edit by Saccavino (28/10/08): use the STL's sort algorithm
	//////////////////////////////////// 

	struct sort_with_permutation {
		vector<REAL> const& v;
		sort_with_permutation(vector<REAL> const& v) : v(v) { }
		bool operator()(int i, int j) const { return v[i] < v[j]; }
	};

	vector<int> Math::sortWithPermutation(vector<REAL>& vec)
	{
		// Generate the array of indices of elements of vec
		vector<int> sigma(vec.size());
		for (int i = 0; i < vec.size(); i++)
			sigma[i] = i;

		// Sort the indices according to the value of the corresponding element of vec
		std::sort(sigma.begin(), sigma.end(), sort_with_permutation(vec));

		// Generate the sorted vector and copy it in vec
		vector<REAL> vec_sorted(vec.size());
		for (int i = 0; i < vec.size(); i++)
			vec_sorted[i] = vec[sigma[i]];
		vec.swap(vec_sorted);

		return sigma;
	}

	//////////////////////////////////// 
	/// Computes the intersection of the two interval vectors.
	//////////////////////////////////// 

	vector<REAL> Math::intervalsIntersection(vector<REAL> const& i1, vector<REAL> const& i2)
	{
		vector<REAL> intersection(0);
		vector<REAL> trueIntersection(0);
		int i,j,n1,n2;

		n1 = i1.size();
		if(n1%2!=0)
			return trueIntersection;
		n2 = i2.size();
		if(n2%2!=0)
			return trueIntersection;
		for(i=0; i<n1; i+=2) {
			for(j=0; j<n2; j+=2) {
				if(i1[i]<=i2[j]) {
					if(i2[j+1]<=i1[i+1]) {
						intersection.push_back(i2[j]);
						intersection.push_back(i2[j+1]);
					} else if(i2[j]<=i1[i+1]) {
						intersection.push_back(i2[j]);
						intersection.push_back(i1[i+1]);
					}
				} else {
					if(i1[i+1]<=i2[j+1]) {
						intersection.push_back(i1[i]);
						intersection.push_back(i1[i+1]);
					} else if(i1[i]<=i2[j+1]) {
						intersection.push_back(i1[i]);
						intersection.push_back(i2[j+1]);
					}
				}
			}
		}
		/// Eliminates eventually degenerate intervals.
		for(i=0; i<intersection.size(); i+=2) {
			if(intersection[i+1]-intersection[i] > TOLERANCE) {
				trueIntersection.push_back(intersection[i]);
				trueIntersection.push_back(intersection[i+1]);
			}
		}
		return trueIntersection;
	}

	//////////////////////////////////// 
	/// Computes the factorial of an integer number, n.
	//////////////////////////////////// 

	int Math::factorial(int n)
	{
		if(n==0)
			return 1;
		int answer=1;
		for(int i=1; i<=n; i++) {
			answer *= i;
		}
		return answer;
	}

	//////////////////////////////////// 
	/// Truncates the given REAL number to the required precision
	/// (number of digits).
	/// The parameter "type" gives the type of approximation:
	///   0: "round"
	///   1: "ceil"
	///  -1: "floor"
	//////////////////////////////////// 

	REAL Math::truncate(REAL x, int nDigits, int type)
	{
		int exp,i;
		bool sign;
		double mantissa;

		sign = true;
		/// If x is negative executes the computation on -x.
		if(x < 0.) {
			sign = false;
			type = -type;
			x = -x;
		}
		/// Computes the exponent and mantissa of x.
		mantissa = (double)x;
		exp = 0;
		while(mantissa >= 1.) {
			mantissa *= 0.1;
			exp++;
		}
		while(mantissa < 0.1) {
			mantissa *= 10.;
			exp--;
		}
		for(i=0; i<nDigits; i++) {
			mantissa *= 10.;
			exp--;
		}
		/// Depending on the type executes the truncation.
		if(type == 0)
			mantissa = floor(mantissa+0.5);
		else if(type == 1)
			mantissa = ceil(mantissa);
		else
			mantissa = floor(mantissa);
		/// Takes back the exponent to 0.
		while(exp > 0) {
			mantissa *= 10.;
			exp--;
		}
		while(exp < 0) {
			mantissa *= 0.1;
			exp++;
		}
		if(!sign)
			mantissa = -mantissa;
		return (REAL)mantissa;
	}

	//////////////////////////////////// 
	/// Computes the sign of a REAL a (1 if a>0; 0 if a=0; -1 if a<0)
	//////////////////////////////////// 

	int Math::sign(REAL a)
	{
		if (a>0) return 1;
		else if (a==0) return 0;
		else return -1;
	}

	//////////////////////////////////// 
	/// Computes the root(s) of the quadratic equation
	///  a x^2 + b x + c = 0
	/// with real coefficients a,b,c. Returns the number of (real) solutions.
	//////////////////////////////////// 

	int Math::quadraticRoots(const REAL& a, const REAL& b, const REAL& c, REAL& x1, REAL& x2)
	{
		double k= 4.*a*c/(b*b);

		// If b is zero (or very small compared to a and c)
		// solve the equation a x^2 + c = 0
		if ( ! isfinite(k) )
		{
			double y= -c/a;
			if (y > 0)
			{
				x1= sqrt(y);
				x2= -x1;
				return 2;
			}
			// If -c/a is negative but small, we return 1 solution
			else if (fabs(y) <= Math::Epsilon)
			{
				x1= x2= 0;
				return 1;
			}
			else
			{
				return 0;
			}
		}

		//---------------------------------------
		// Analitic solution:
		//                      _______ 
		//          - b (1 +- \/ 1 - k  )
		//  x    = ----------------------
		//   1,2            2 a

		// k can easily be compared to 1. 

		//---------------------------------------
		// Case |k - 1| << 1
		// Even if 1 - k is negative (but small) we return 1 solution
		if( fabs(k - 1.) <= Math::Epsilon )
		{
			x1= x2= -b/(2.*a);

			return 1;
		}
		//---------------------------------------
		// Case k > 1
		// No solutions...
		else if( k > 1. )
		{
			return 0;
		}

		//---------------------------------------
		// Generic case
		// The solution with the - sign is calculated with higher precision if we multiply by 
		//   	  _______
		//  1 + \/ 1 - k                          - 2 c  
		// _______________       =>   x  = ____________________
		//        _______              1              _______
		//  1 + \/ 1 - k                    b ( 1 + \/ 1 - k  )

		double s = sqrt(1.-k);
		x1= -2.*c/(b*(1+s));
		x2= -b*(1+s)/(2.*a);

		return 2;
	}

	/////////////////////////////////////////////////////////////////////////
	/// Checks if the triangle and a segment are (partially) overlapped.
	/// The overlapping segment is saved in "intersection".
	/// Returns the number of points saved in "intersection".
	/////////////////////////////////////////////////////////////////////////
	int Math::triangleSegmentOverlap(Real2D tri[3], Real2D seg[2], Real2D intersection[2])
	{
		bool startBelongsToTriangle = seg[0].belongsToTriangle(tri[0], tri[1], tri[2]);
		bool endBelongsToTriangle = seg[1].belongsToTriangle(tri[0], tri[1], tri[2]);
		if( startBelongsToTriangle && endBelongsToTriangle )
		{
			intersection[0] = seg[0];
			intersection[1] = seg[1];
			return 2;
		}

		int countIntersection = 0;
		for(int i = 0; i < 3; i++)
		{
			Real2D inters;
			int iPlus1 = (i+1)%3;
			int intResult = Real2D::computeIntersection(tri[i], tri[iPlus1], seg[0], seg[1], inters);
			if( intResult == 2 ) // superposition
			{
				Real2D dir = tri[iPlus1] - tri[i];

				// lista ordinata (per parametro lungo la direzione dir) dei 4 punti
				std::list<std::pair<double,Real2D> > points;
				points.push_back( std::make_pair((double)0., tri[i]) );
				points.push_back( std::make_pair(dir*dir, tri[iPlus1]) );

				for(int j = 0; j < 2; j++)
				{
					double param = dir * (seg[j]-tri[i]);
					std::list<std::pair<double,Real2D> >::iterator it = points.begin();
					while( it != points.end() )
					{
						if( param < it->first )
						{
							points.insert(it, std::make_pair(param, seg[j]) );
							break;
						}
						it++;
					}
					if( it == points.end() )
						points.push_back(std::make_pair(param, seg[j]) );
				}

				// il segmento di overlap è quello compreso tra il secondo e il terzo punto
				std::list<std::pair<double,Real2D> >::iterator it = points.begin()++;
				intersection[0] = it->second;
				it++;
				intersection[1] = it->second;
				return 2;
			}
			else if( intResult == 1 ) // intersection
			{
				intersection[countIntersection] = inters;
				countIntersection++;

				// se ho già trovato due punti di intersezione, ho il segmento di overlap
				if( countIntersection == 2 )
					return 2;
			}
		}

		// se ho trovato un punto solo, uno degli estremi del segmento "dovrebbe" essere contenuto nel triangolo
		if( countIntersection == 1 )
		{
			if( startBelongsToTriangle )
			{
				intersection[1] = seg[0];
				return 2;
			}
			else if( endBelongsToTriangle )
			{
				intersection[1] = seg[1];
				return 2;
			}

			return 1;
		}

		return 0;
	}

	//////////////////////////////////// 
	/// Computes the intersection of two convex polygons.
	/// Returns the number of vertices of the intersection polygon.
	/// Implements (revisited and corrected) the ideas of
	/// O'Rourke-Chien-Olson-Naddor (1982), see Preparata-Shamos pp. 273-276.
	//////////////////////////////////// 

	/// If the last point of retail coincides with the previous one skips it.
	/// If the last point of retail coincides with the first one stops.

#define ADD_TO_RETAIL(X) \
	do { \
	Real2D x = (X); \
	if((l>=3) && (x.equals(retail[0],2*tol))) \
	goto the_village; \
	if(!x.equals(retail[l-1],2*tol)) \
	retail[l++]=x; \
	} while(0)

	int Math::computeConvexPolygonsIntersection(Real2D* p, int np,
		Real2D* q, int nq,
		Real2D* retail)
	{
		int i,j,k,l;
		REAL p0,p1,q0,q1,t,tol;
		Real2D center(0,0);
		bool QbelongsToP, PbelongsToQ;

		i = j = k = l = 0;
		p0 = p1 = q0 = q1 = t = 0;
		/// 	tol = fabs((q[2]-q[0])^(q[1]-q[0]));
		/// 	tol = fabs((p[2]-p[0])^(p[1]-p[0]));
		tol = Math::TOLERANCE2;
		/// Computes the intersection points between the perimeters of "p" and "q".
		do {
			i = i%np;
			j = j%nq;
			k++;
			p0 = (q[j] - q[(j-1+nq)%nq])^(p[(i-1+np)%np] - q[(j-1+nq)%nq]);
			p1 = (q[j] - q[(j-1+nq)%nq])^(p[i] - q[(j-1+nq)%nq]);
			q0 = (p[i] - p[(i-1+np)%np])^(q[(j-1+nq)%nq] - p[(i-1+np)%np]);
			q1 = (p[i] - p[(i-1+np)%np])^(q[j] - p[(i-1+np)%np]);

			if(fabs(p0) < tol && fabs(p1) < tol) {
				Real2D v = q[j] - q[(j-1+nq)%nq];
				if(v*(p[i] - p[(i-1+np)%np]) < 0.) // the two edges are opposite
					return 0;
				else if( v*(p[i] - q[(j-1+nq)%nq]) < 0.) // position of p1 w.r.t. q0
					j++;
				else if( v*(p[(i-1+np)%np] - q[j]) > 0.) // position of p0 w.r.t. q1
					i++;
				else if( v*(p[i] - q[j]) < 0) // position of p1 w.r.t. q1
					ADD_TO_RETAIL(p[i++]);
				else
					ADD_TO_RETAIL(q[j++]);
			} else if(p0 < 0 && p1 < 0) // both p[i] and p[i-1] lie outside h(q)
				i++; // advance p
			else if(q0 < 0 && q1 < 0) // both q[j] and q[j-1] lie outside h(p)
				j++; // advance q
			else if(p0 > -tol && p1 < tol) { // p[i] lies outside and p[i-1] lies inside h(q)
				if(q0 < tol && q1 > -tol) { // the two edges intersect
					t = q0/(q0 - q1); // computes the intersection point and adds it to the retail
					ADD_TO_RETAIL(q[(j-1+nq)%nq] + t*(q[j] - q[(j-1+nq)%nq]));
				}
				i++; // advance p
			} else if(q0 > -tol && q1 < tol) { // q[j] lies outside and q[j-1] lies inside h(p)
				if(p0 < tol && p1 > -tol) { // the two edges intersect
					t = q0/(q0 - q1); // computes the intersection point and adds it to the retail
					ADD_TO_RETAIL(q[(j-1+nq)%nq] + t*(q[j] - q[(j-1+nq)%nq]));
				}
				j++; // advance q
			} else if(p0 < tol && p1 > -tol) { // p[i-1] lies outside and p[i] lies inside h(q)
				if(l) ADD_TO_RETAIL(q[j]);
				j++; // advance q
			} else if(q0 < tol && q1 > -tol) { // q[j-1] lies outside and q[j] lies inside h(p)
				if(l) ADD_TO_RETAIL(p[i]);
				i++; // advance p
			} else i++;
		} while(k <= 2*(np + nq) && l < np + nq);
the_village:

		/// If the intersection polygon is not degenerate returns the number of sides.
		if(l > 2)
			return l;

		/// If no intersection at all has been found checks if one of the two polygons
		/// is completely contained into the other.

		/// Checks if the center of Q belongs to P.
		QbelongsToP = true;
		for(j=0; j<nq; j++)
			center += q[j];
		center /= nq;
		for(i=0; i<np; i++) {
			if(((p[(i+1)%np] - p[i])^(center - p[i])) < 0.) {
				QbelongsToP = false;
				break;
			}
		}
		if(QbelongsToP) {
			for(j=0; j<nq; j++)
				retail[j] = q[j];
			return nq;
		}

		/// Checks if p[0] belongs to Q.
		PbelongsToQ = true;
		center = Real2D(0,0);
		for(i=0; i<np; i++)
			center += p[i];
		center /= np;
		for(j=0; j<nq; j++) {
			if(((q[(j+1)%nq] - q[j])^(center - q[j])) < 0.) {
				PbelongsToQ = false;
				break;
			}
		}
		if(PbelongsToQ) {
			for(i=0; i<np; i++)
				retail[i] = p[i];
			return np;
		}
		return 0;
	}

	//////////////////////////////////// 
	/// Computes the convex hull of the data points stored in "points",
	/// points is a 2*size double stream
	//////////////////////////////////// 

	int Math::compute2DConvexHull(int size, REAL *points, Real2D*& hull, double *scale)
	{
		int i,j;	
		REAL minY;
		Real2D bottomPoint;
		Real2D maxLeft;
		Real2D maxRight;
		Real2D maxTop;
		Real2D maxBottom;
		Real2D serv2D((REAL)0.,(REAL)0.);
		vector<Real2D> pointVector(0);
		vector<Real2D> sortedPoints(0);
		vector<bool> belongToHull(0);
		vector<Real2D> convexHull(0);	

		/// Stores data in the pointVector.
		for(i=0;i<size;i++){
			serv2D.v[0] = points[2*i];
			serv2D.v[1] = points[2*i+1];
			pointVector.push_back(serv2D);		
		}
		/// Finds the points with extreme coordinates
		maxTop = pointVector[0];
		maxBottom = pointVector[0];
		maxLeft = pointVector[0];
		maxRight = pointVector[0];
		for(i=1;i<pointVector.size();i++){
			if(pointVector[i].v[1] > maxTop.v[1]){
				maxTop = pointVector[i];
			}
			if(pointVector[i].v[1] < maxBottom.v[1]){
				maxBottom = pointVector[i];
			}
			if(pointVector[i].v[0] < maxLeft.v[1]){
				maxLeft = pointVector[i];
			}
			if(pointVector[i].v[0] > maxRight.v[1]){
				maxRight = pointVector[i];
			}
		}
		(*scale) = fabs(0.5*((maxTop-maxBottom).mod()+(maxRight-maxLeft).mod()));
		/// Finds the rightest bottom point in the set (pivot point).
		bottomPoint = pointVector[0];
		minY = pointVector[0].v[1];
		for(i=1;i<pointVector.size();i++){
			if(pointVector[i].v[1] < minY){
				minY = pointVector[i].v[1];
				bottomPoint = pointVector[i];
			}
			else if(pointVector[i].v[1] == minY){
				if(pointVector[i].v[0] >= bottomPoint.v[0]){
					bottomPoint = pointVector[i];
				}
			}
		}	
		/// Finds for each point its distance and angle from the pivot point.	
		vector<REAL> dist(pointVector.size());
		vector<REAL> theta(pointVector.size());
		for(i=0;i<pointVector.size();i++){		
			dist[i] = (pointVector[i] - bottomPoint).mod();
			if(dist[i]!=0){
				theta[i] = acosR((pointVector[i].v[0] - bottomPoint.v[0])/dist[i]);
				if((pointVector[i].v[1] - bottomPoint.v[1])<0){
					theta[i] = Math::TwoPi - theta[i];
				}
			}
			else
				theta[i] = 0.;		
		}

		/// Orders the points following angle and distance criteria (n*n operations).
		int minIndex;
		REAL minDist;
		REAL minTheta;
		bool someFound;

		sortedPoints.push_back(bottomPoint);
		belongToHull.push_back(true);
		for(j=0;j<pointVector.size();j++){
			minTheta = Math::TwoPi + 1.0;
			minDist = Math::LargeReal;
			someFound = false;
			for(i=0;i<pointVector.size();i++){		
				if(dist[i] > 0.0){				
					if(theta[i] < minTheta){
						minTheta = theta[i];
						minDist = dist[i];
						minIndex = i;
						someFound = true;
					}
					else if(theta[i] == minTheta){
						if(dist[i] < minDist){
							minTheta = theta[i];
							minDist = dist[i];
							minIndex = i;
							someFound = true;
						}
					}
				}
			}
			if(someFound == true){
				sortedPoints.push_back(pointVector[minIndex]);
				belongToHull.push_back(true);
				theta[minIndex] = 0.0;
				dist[minIndex] = 0.0;
			}
			else
				break;
		}		
		/// Computation of the convex hull.
		/// 1) Clears repeated points from the convex hull.	
		for(i=0;i<sortedPoints.size()-1;i++){
			if((sortedPoints[i+1] - sortedPoints[i]).mod()< Math::TOLERANCE){
				belongToHull[i+1] = false;			
			}
			else{			
			}
		}	
		/// 2) Performs the iteration.	
		int w;	
		Real2D vec1,vec2;
		for(i=2;i<sortedPoints.size();i++){
			if(belongToHull[i] == true){
				for(j=i-1;j>0;j--){
					if(belongToHull[j] == true){
						w = j-1;
						while(belongToHull[w] == false){
							w--;
						}				
						vec1 = sortedPoints[j] - sortedPoints[w];
						vec2 = sortedPoints[i] - sortedPoints[w];	
						if((vec1.v[0]*vec2.v[1] - vec1.v[1]*vec2.v[0]) > 0){						
						}
						else{
							belongToHull[j] = false;
						}
					}	
				}
			}
		}	
		/// Stores the convex hull in the array
		int counter = 0;
		for(i=0; i<sortedPoints.size(); i++) {
			if(belongToHull[i] == true){
				counter++;
			}
		}	
		hull = new Real2D[counter];
		counter = 0;
		for(i=0; i<sortedPoints.size(); i++) {
			if(belongToHull[i] == true){			
				hull[counter] = sortedPoints[i];
				counter++;
			}
		}	
		return counter;
	}

	////////////////////////////////////// 
	/// Extrudes the convex hull surface to a distance leng wih aperture alpha
	////////////////////////////////////// 
	int Math::extrudeConvexHullAlongZ(int size, Real2D* hull, REAL leng, REAL alpha, Real2D*& extrudedHull)
	{
		int i;
		REAL dist;
		REAL tangent;
		if((alpha<0)||(alpha>=Math::PiDiv2)){
			return 0;
		}
		tangent = sin(alpha)/cos(alpha);
		dist = leng*tangent;
		Real2D diff;
		Real2D baricenter(0,0);
		for(i=0;i<size;i++){
			baricenter += hull[i];
		}
		baricenter /= size;
		extrudedHull = new Real2D[size];
		for(i=0;i<size;i++){
			diff = hull[i] - baricenter;
			extrudedHull[i] = baricenter + diff.vers()*(diff.mod()+dist);
		}
		return size;
	}

	//////////////////////////////////////
	// Check box intersection in 2D
	// Se esiste almeno una direzione i in cui gli intervalli [aMin[i], aMax[i]] e
	// [bMin[i], bMax[i]] sono separati allora i box hanno intersezione nulla 
	//////////////////////////////////////
	bool Math::checkBoxIntersection(
		Real2D& aMin, Real2D& aMax,
		Real2D& bMin, Real2D& bMax)
	{
		for(int i= 0; i<2; i++)
		{
			if(aMax[i] < bMin[i]) return false;
			if(bMax[i] < aMin[i]) return false;
		}
		return true;
	}

	//////////////////////////////////////
	// Compute normals of regular mesh formed by quadrangles
	//////////////////////////////////////
	/*  
	Internal vertices
	---- b ----
	|    .   .    |
	|   .  |  .   |
	|  .   |   .  |
	| .    |    . |
	.     |     .
	c ---- p ---- a 
	.     |     .
	| .    |    . |
	|  .   |   .  |
	|   .  |  .   |
	|    .   .    |
	---- d ----

	area-weighted normal at p (to be normalized) =      
	= (a-p)^(b-p) + (b-p)^(c-p) + (c-p)^(d-p) + (d-p)^(a-p) =
	= a^b + b^c + c^d + d^a =
	(a-c)^(b-d)

	For extremal vertices, es: no c and no d, put c = p and d = p and apply the same formula, ecc
	*/
	template<class T>
	void ComputeNormalsOfRegularMesh(
		int nU, int nV, vector<T> &verts,
		vector<T> &normal)
	{
		assert( nU > 0 && nV > 0 );
		int dim = ((int)verts.size())/(nU*nV);

		assert( dim == 3 || dim == 4 );

		normal.resize( 3*nU*nV );

		T d1[3], d2[3], n[3], m;
		int i, j, k,  ia, ib, ic, id;
		for(i=0; i<nU; i++)
		{
			for(j= 0; j<nV; j++)
			{
				if(i>0 && i<nU-1) { ia= dim*(i+1+j*nU); ic= dim*(i-1+j*nU); }
				else if(i==0)     { ia= dim*(i+1+j*nU); ic= dim*(i  +j*nU); }
				else              { ia= dim*(i  +j*nU); ic= dim*(i-1+j*nU); }

				if(j>0 && j<nV-1) { ib= dim*(i+(j+1)*nU); id= dim*(i+(j-1)*nU); }
				else if(j==0)     { ib= dim*(i+(j+1)*nU); id= dim*(i    +j*nU); }
				else              { ib= dim*(i    +j*nU); id= dim*(i+(j-1)*nU); }
				if(dim==3)
					for(k= 0; k<3; k++)
					{
						d1[k]= verts[ia+k]-verts[ic+k];
						d2[k]= verts[ib+k]-verts[id+k];
					}
				else
					for(k= 0; k<3; k++)
					{
						d1[k]= verts[ia+k]/verts[ia+3] - verts[ic+k]/verts[ic+3];
						d2[k]= verts[ib+k]/verts[ib+3] - verts[id+k]/verts[id+3];
					}

					n[0]=  d1[1]*d2[2] - d1[2]*d2[1];
					n[1]= -d1[0]*d2[2] + d1[2]*d2[0];
					n[2]=  d1[0]*d2[1] - d1[1]*d2[0];

					m=	n[0]*n[0] + n[1]*n[1] + n[2]*n[2]; m= sqrt(m);
					if( m != 0 )
					{
						normal[3*(i+j*nU)  ]= n[0]/m;
						normal[3*(i+j*nU)+1]= n[1]/m;
						normal[3*(i+j*nU)+2]= n[2]/m;
					}
					else
					{
						normal[3*(i+j*nU)  ]= 0;
						normal[3*(i+j*nU)+1]= 0;
						normal[3*(i+j*nU)+2]= 1;
					}
			}
		}
	}

	//////////////////////////////////////
	// Compute normals of regular mesh formed by quadrangles
	//////////////////////////////////////
	void Math::computeNormalsOfRegularMesh(
		int nU, int nV, vector<float> &verts,
		vector<float> &normal)
	{
		return ComputeNormalsOfRegularMesh<float>(nU, nV, verts, normal);
	}

	//////////////////////////////////////
	// Compute normals of regular mesh formed by quadrangles
	//////////////////////////////////////
	void Math::computeNormalsOfRegularMesh(
		int nU, int nV, vector<double> &verts,
		vector<double> &normal)
	{
		return ComputeNormalsOfRegularMesh<double>(nU, nV, verts, normal);
	}

	//////////////////////////////////////
	// Returns the distance between the point p and the segment ab (2D)
	//////////////////////////////////////
	REAL Math::distancePointSegment(const Real2D &p, const Real2D &a, const Real2D &b, Real2D *q)
	{
		Real2D pa = p - a;
		Real2D ba = b - a;
		REAL x = pa * ba;
		if (x < 0)
		{
			if (q) *q = a;
			return pa.mod();
		}
		REAL ba2 = ba.sqrmod();
		if (x < ba2)
		{
			if (q) *q = a + (x/ba2) * ba;
			return fabs(pa^ba) / sqrt(ba2);
		}
		else
		{
			if (q) *q = b;
			return (p - b).mod();
		}
	}

	////////////////////////////////////////////////////////////////////////////
	/// Returns the distance between the segment ab and the segment cd (2D)
	////////////////////////////////////////////////////////////////////////////
	REAL Math::distanceSegmentSegment(const Real2D &a, const Real2D &b, const Real2D &c, const Real2D &d, Real2D *p, Real2D *q)
	{
		// First check if the segments intersect each other
		Real2D ba = b - a;
		REAL side_c = (c-a)^ba;
		REAL side_d = (d-a)^ba;
		if (side_c*side_d < 0)
		{
			Real2D dc = d - c;
			REAL side_a = (a-c)^dc;
			REAL side_b = (b-c)^dc;
			if (side_a*side_b < 0)
			{
				// Return the intersection point between the (infinite) lines
				if (p || q)
				{
					Real2D inters = ((c^d)*ba - (a^b)*dc) / (ba^dc);
					if (p) *p = inters;
					if (q) *q = inters;
				}
				return 0;
			}
		}

		// Return the minimum distance between the 4 possible (vertex,segment) pairs
		Real2D temp;

		// p = a, line = cd
		REAL mindist = distancePointSegment(a, c, d, &temp);
		if (p) *p = a;
		if (q) *q = temp;

		// p = b, line = cd
		REAL dist = distancePointSegment(b, c, d, &temp);
		if (dist < mindist) {
			mindist = dist;
			if (p) *p = b;
			if (q) *q = temp;
		}

		// p = c, line = ab
		dist = distancePointSegment(c, a, b, &temp);
		if (dist < mindist) {
			mindist = dist;
			if (p) *p = temp;
			if (q) *q = c;
		}

		// p = d, line = ab
		dist = distancePointSegment(d, a, b, &temp);
		if (dist < mindist) {
			mindist = dist;
			if (p) *p = temp;
			if (q) *q = d;
		}

		return mindist;
	}

	//////////////////////////////////////
	// Returns the angle between two vectors in 2D space.
	// The angle is measured in radians (moving in counterclockwise direction
	// from 'v' to 'w') and is between -Pi and Pi.
	//////////////////////////////////////
	REAL Math::angleBetween(const Real2D &v, const Real2D& w)
	{
		REAL phi = atan2(w[1],w[0]) - atan2(v[1],v[0]);
		if (phi <= -Math::Pi) phi += Math::TwoPi;
		if (phi > Math::Pi) phi -= Math::TwoPi;
		return phi;
	}

	//////////////////////////////////////////////////////////////////////
	// Compute the (x,y) values for a distribution graph
	// The values of the variable to plot are given in "data", and
	// minVal and maxVal must be the minimum and maximum value.
	// If "nPoints" is zero, an appropriate number of points is computed.
	//////////////////////////////////////////////////////////////////////
	void Math::ComputeDistributionValues(const vector<float>& data,
		REAL minVal,
		REAL maxVal,
		vector<float>& x,
		vector<float>& y,
		int nPoints)
	{
		x.clear();
		y.clear();
		if (data.empty())
		{
			// No data to plot
			return;
		}

		if (!nPoints)
		{
			// Compute a good number of steps
			int N = (int)(2*log10((double)data.size()) + 1);
			if (N < 1) N = 1;
			if (N > 11) N = 11;
			nPoints = (1 << N);
		}

		// Fill the arrays
		REAL delta = (maxVal - minVal) / nPoints;
		x.resize(2*nPoints);
		y.resize(2*nPoints,0.0f);

		for (int i = 0; i < nPoints; i++)
		{
			x[2*i] = (float)(minVal + i*delta);
			x[2*i+1] = (float)(minVal + (i+1)*delta);
		}
		for (size_t i = 0; i != data.size(); i++)
		{
			int pos = (int)((data[i] - minVal) / delta);
			if (pos < 0) pos = 0;
			if (pos >= nPoints) pos = nPoints-1;
			y[2*pos] += 1.0f;
			y[2*pos+1] += 1.0f;
		}
	}

	// Get the angle between this line and a target line.
	double angleBetween(const Real2D& source_line, const Real2D& target_line) const
	{
		double a = Math::polarPhi2D(source_line);
		double b = Math::polarPhi2D(target_line);
		double diff = a - b;
		while (diff > Math::Pi) diff -= Math::TwoPi;
		while (diff <= -Math::Pi) diff += Math::TwoPi;
		return Math::toDeg(diff);
	}
	
	bool Math::polygonContains(std::vector<Real2D> const& vertex, Real2D const& test)
	{
		double l_angleTot = 0.;
		for(size_t i = 0; i < vertex.size(); ++i)
		{
			l_angleTot+= angleBetween(vertex[i] - test, vertex[(i+1) % vertex.size()] - test);
		}

		return fabs(l_angleTot Math::TwoPi) < Math::TOLERANCE;
	}

}
