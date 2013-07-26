// nostop_box.hpp: Bounding box class
//
//////////////////////////////////////////////////////////////////////

#ifndef NOSTOP_BOUNDING_BOX_HPP
#define NOSTOP_BOUNDING_BOX_HPP
#pragma once

#include "nostop_real2D.h"

namespace NoStop
{
	/// This class represents an axis-aligned rectangular bounding box.
	class Box
	{
	public:
		Real2D minCoord;
		Real2D maxCoord;

		/// Construct an empty box.
		Box() {
			minCoord[0] = minCoord[1] = Math::LargeReal;
			maxCoord[0] = maxCoord[1] = -Math::LargeReal;
		}

		/// Construct a box containing a single point.
		Box(const Real2D &point) {
			minCoord = point;
			maxCoord = point;
		}

		/// Construct a box containing a single point.
		Box(const Real2D &mincoord, const Real2D &maxcoord) {
			minCoord = mincoord;
			maxCoord = maxcoord;
		}

		/// Returns the box's center.
		Real2D center() const {
			return (minCoord + maxCoord) * 0.5;
		}

		/// Returns the box's corners. Indices range from 0 to 7.
		Real2D corner(int i) const {
			return Real2D(
				i & 1 ? maxCoord[0] : minCoord[0],
				i & 2 ? maxCoord[1] : minCoord[1]);
		}

		/// Returns the length of box's sides, in the three directions.
		Real2D size() const {
			return maxCoord - minCoord;
		}

		/// Tests if the box is empty.
		bool empty() const {
			return minCoord[0] > maxCoord[0] || minCoord[1] > maxCoord[1];
		}

		/// Tests if the box is finite.
		bool finite() const {
			return minCoord.isfinite() && maxCoord.isfinite();
		}

		/// Translate the box.
		Box operator+(const Real2D &point) const {
			Box box;
			box.minCoord = minCoord + point;
			box.maxCoord = maxCoord + point;
			return box;
		}

		/// Extends the box so that it contains the given point.
		Box &extend(const Real2D &point) {
			for (int i = 0; i < 2; i++) {
				if (minCoord[i] > point[i]) minCoord[i] = point[i];
				if (maxCoord[i] < point[i]) maxCoord[i] = point[i];
			}
			return *this;
		}

		/// Extends the box so that it contains the other box.
		Box &extend(const Box &box) {
			for (int i = 0; i < 2; i++) {
				if (minCoord[i] > box.minCoord[i]) minCoord[i] = box.minCoord[i];
				if (maxCoord[i] < box.maxCoord[i]) maxCoord[i] = box.maxCoord[i];
			}
			return *this;
		}

		/// Increases the size of the box by a certain amount in all directions.
		Box &fatten(REAL delta) {
			for (int i = 0; i < 2; i++) {
				minCoord[i] -= delta;
				maxCoord[i] += delta;
			}
			return *this;
		}

		/// Tests if the box contains the given point, within the given tolerance.
		bool contains(const Real2D &point, REAL tolerance = 0.) const {
			for (int i = 0; i < 2; i++) {
				if (minCoord[i] - tolerance > point[i]) return false;
				if (maxCoord[i] + tolerance < point[i]) return false;
			}
			return true;
		}

		/// Tests if the boxes overlap within the given tolerance.
		bool overlaps(const Box &box, REAL tolerance = 0.) const {
			for (int i = 0; i < 2; i++) {
				if (minCoord[i] - 2*tolerance > box.maxCoord[i]) return false;
				if (maxCoord[i] + 2*tolerance < box.minCoord[i]) return false;
			}
			return true;
		}

		/// Returns the squared distance fron a point.
		REAL squaredDistance(const Real2D &point) const {
			REAL dist2 = 0;
			for (int i = 0; i < 2; i++) {
				REAL r = minCoord[i] - point[i];
				if (r > 0) dist2 += r*r;
				r = point[i] - maxCoord[i];
				if (r > 0) dist2 += r*r;
			}
			return dist2;
		}
	};
}

#endif // NOSTOP_BOUNDING_BOX_H
