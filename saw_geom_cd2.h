// saw_geom_prim2.h - Basic math on 2d primitives
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>

//-----------------------------------------------------------------------------------------------------------
// History
// - v1.09 - 03/12/16 - Added squared distance for P <-> P, P <-> Ls, P <-> C, C <-> C, C <-> P, Ls <-> P
//                    - Cd2CLs/Cd2LsC returns intersection point
// - v1.08 - 05/27/13 - Cd2LsLs returns intersection point
// - v1.07 - 04/21/13 - Small code and comment changes for clarity
// - v1.06 - 05/22/12 - Added PolarToXy2 and XyToPolar2
// - v1.05 - 05/11/12 - Moved to "glory" namespace. Will be part of a generic 2d game library.
// - v1.04 - 11/02/11 - Fixed obb to aabb when obb halfW or halfH is negative
//                    - Fixed obb to aabb typo (caused 100% incorrect results)
//                    - Fixed point to line seg
//                    - Fixed point to obb when obb halfW or halfH is negative.
//                    - Correctness testing done. Fixed all known algorithmic bugs.
//                    - Fixed circle to circle if either radius is negative
//                    - Fixed circle to circle formula (incorrect formula!)
//                    - Added check in circle to line segment for line segments with 0 length (points)
//                    - Fix circle to obb when obb halfW or halfH is negative
//                    - Fix obb to triangle when obb halfW or halfH is negative
//                    - Fix obb to lineseg when obb halfW or halfH is negative
//                    - Fix algorithm in obb to obb - forgot to update vars. halfW and halfH on 2nd pass
// - v1.03 - 10/14/11 - Finished adding all combinations of primitives for collision detection
// - v1.02 - 10/09/11 - Added Normalize2, Aabb <-> Ls, Obb <-> Ls, P <-> Ls, P <-> P
// - v1.01 - 09/29/11 - Added Rotate2 (simple extension of Project2)
// - v1.00 - 09/22/11 - Initial release by achild

//-----------------------------------------------------------------------------------------------------------
// Notes
// - Primitives 
//   - Point2, Aabb2, Obb2, LineSeg2, Triangle2, Circle2
//
// - Utility 
//   - CalcLen2, Normalize2, CalcDot2, IsClockwise2, Project2, Unproject2, Rotate2, PolarToXy2, XyToPolar2
//
// - Collision Detection 
//   - Any primitive with any primitive
//     Convex N-sided with Convex N-sided
//   - (Collision detection only; NOT collision response)
//   - (Some methods are naive)
//
// - Squared Distance
//   - Point <-> Point, Point <-> Circle, Point <-> LineSeg, Circle <-> Circle

//-----------------------------------------------------------------------------------------------------------
// Todo
// @@ Haven't considered divide-by-zero issues.
// @@ Haven't really considered denormals.
// http://user.cockos.com/~deadbeef/index.php?search=denormal

#ifndef _SAW_GEOM_CD2_INCLUDED
#define _SAW_GEOM_CD2_INCLUDED

#include <math.h>

namespace sawg {

//-----------------------------------------------------------------------------------------------------------
// PRIMITIVES
//-----------------------------------------------------------------------------------------------------------

struct Point2 {
	float x, y;
	Point2() { }
	Point2(float x, float y) : x(x), y(y) { }
};

//-----------------------------------------------------------------------------------------------------------
struct Aabb2 {
	float minX, minY, maxX, maxY;
	Aabb2() { }
	Aabb2(float minX, float minY, float maxX, float maxY) : minX(minX), maxX(maxX), minY(minY), maxY(maxY) { }
	bool operator==(const Aabb2 &rhs) const { return minX == rhs.minX && minY == rhs.minY && maxX == rhs.maxX && maxY == rhs.maxY; }
	bool operator!=(const Aabb2 &rhs) const { return !(*this == rhs); }
	float GetArea() const { return fabs((maxX - minX) * (maxY - minY)); }
};

//-----------------------------------------------------------------------------------------------------------
struct Obb2 {
	float cx, cy;
	float orientX, orientY;  // Orientation of 0 degrees would be 1, 0 and goes ccw. MUST BE NORMALIZED.
	float halfW, halfH;
	Obb2() { }
	Obb2(float cx, float cy, float orientX, float orientY, float halfW, float halfH)
		: cx(cx), cy(cy), orientX(orientX), orientY(orientY), halfW(halfW), halfH(halfH) { }
	float GetArea() const { return fabs(halfW * halfH * 4); }
};

//-----------------------------------------------------------------------------------------------------------
struct LineSeg2 {
	float x1, y1, x2, y2;
	LineSeg2() { }
	LineSeg2(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2) { }
};

//-----------------------------------------------------------------------------------------------------------
struct Triangle2 {
	float x1, y1, x2, y2, x3, y3;
	Triangle2() { }
	Triangle2(float x1, float y1, float x2, float y2, float x3, float y3) : x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3) { }
	float GetArea() const { return fabs((x2 - x3) * (y1 - y3) - (x1 - x3) * (y2 - y3)) * .5f; }
};

//-----------------------------------------------------------------------------------------------------------
struct Circle2 {
	float x, y, r;
	Circle2() { }
	Circle2(float x, float y, float r) : x(x), y(y), r(r) { }
	float GetArea() const { return 3.1415927f * r * r; }
};

//-----------------------------------------------------------------------------------------------------------
// UTILITY
//-----------------------------------------------------------------------------------------------------------

// Calculate length between 2 points
inline float CalcLen2(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1, dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

// Normalize a point
inline void Normalize2(float x, float y, float *pX, float *pY) {
	float ilen = 1.0f / sqrt(x * x + y * y);
	*pX = x * ilen;
	*pY = y * ilen;
}

// Calculate dot product of 2 points
inline float CalcDot2(float x1, float y1, float x2, float y2) {
	return x1 * x2 + y1 * y2;
}

// Calculate if 3 points are in clockwise order
inline bool IsClockwise2(float x1, float y1, float x2, float y2, float x3, float y3, bool yIsUp) {
	float z = (x1 - x2) * (y3 - y2) - (y1 - y2) * (x3 - x2);
	return yIsUp == (z > 0);
}

// Project x, y TO a different orientation
// ie Go from being axis aligned into a certain orientation
// ie Take a vector from orientation's local space
inline void Project2(float x, float y, float orientX, float orientY, float *pX, float *pY) {
	*pX = x * orientX - y * orientY;
	*pY = x * orientY + y * orientX;
}

// Unproject x, y FROM an orientation (inverse the project operation)
// ie Go from a certain orientation into being axis aligned
// ie Bring a vector into orientation's local space
// This works out to just Project2 with a negative orientY.
inline void Unproject2(float projX, float projY, float orientX, float orientY, float *pX, float *pY) {
	// ox2oy2 is always 1
	//float ox2oy2 = orientX * orientX + orientY * orientY;
	//*pX = (projX * orientX + projY * orientY) / ox2oy2;
	//*pY = (projY - *pX * orientY) / orientX;
	*pX = projX * orientX + projY * orientY;
	*pY = projY * orientX - projX * orientY;
}

// Rotates a point around another point
inline void Rotate2(float cx, float cy, float x, float y, float rad, float *pX, float *pY) {
	float cosRad = cos(rad);
	float sinRad = sin(rad);
	x -= cx;
	y -= cy;
	// This is just the Project2 function plus cx or cy
	*pX = cx + x * cosRad - y * sinRad;
	*pY = cy + y * cosRad + x * sinRad;
}

// Convert radians/distance to x, y
inline void PolarToXy2(float rad, float dist, float *pX, float *pY) {
	*pX = cos(rad) * dist;
	*pY = sin(rad) * dist;
}

// Convert x, y to radians/distance
// Radians output are between -pi and +pi
inline void XyToPolar2(float x, float y, float *pRad, float *pDist) {
	*pRad = atan2(y, x);
	*pDist = sqrt(x * x + y * y);
}

//-----------------------------------------------------------------------------------------------------------
// COLLISION DETECTION
//-----------------------------------------------------------------------------------------------------------
//      A  P  C  Ls T  O  N
//  A   X  X  X  X  X  X
//  P   X  X  X  X  X  X
//  C   X  X  X  X  X  X
//  Ls  X  X  X  X  X  X
//  T   X  X  X  X  X  X
//  O   X  X  X  X  X  X
//  N                     X

// Collision detection 2d: convex n-sided and convex n-sided
inline bool Cd2NN(const float *px1, const float *py1, int n1, const float *px2, const float *py2, int n2) {
	const float BIG_FLT = 1E+37f;
	const float *pvx[2] = { px1, px2 };
	const float *pvy[2] = { py1, py2 };
	int count[2] = { n1, n2 };
	// Test for separating axis on all edges of all polygons
	for (int i = 0; i < 2; i++) {
		int lastJ = count[i] - 1;
		for (int j = 0; j < count[i]; j++) {
			// Get vector of normal of our current test edge 
			// (note because we're comparing, normalizing vector is unneeded)
			float vecx = pvy[i][lastJ] - pvy[i][j];
			float vecy = pvx[i][j] - pvx[i][lastJ];

			// Now project both polygons onto vector, test for overlap
			float mn[2] = { BIG_FLT, BIG_FLT }, mx[2] = { -BIG_FLT, -BIG_FLT };
			for (int k = 0; k < 2; k++) {
				for (int l = 0; l < count[k]; l++) {
					float d = pvx[k][l] * vecx + pvy[k][l] * vecy;
					if (d < mn[k])
						mn[k] = d;
					if (d > mx[k])
						mx[k] = d;
				}
			}
			if (mn[0] > mx[1] || mx[0] < mn[1])
				return false;
			lastJ = j;
		}
	}
	return true;
}

// Collision detection 2d: Aabb and Aabb
inline bool Cd2AA(const Aabb2 &a1, const Aabb2 &a2) {
	return a1.minX <= a2.maxX && a1.maxX >= a2.minX && a1.minY <= a2.maxY && a1.maxY >= a2.minY;
}

// Collision detection 2d: Aabb and Point
inline bool Cd2AP(const Aabb2 &a, const Point2 &p) {
	return (p.x >= a.minX && p.x <= a.maxX && p.y >= a.minY && p.y <= a.maxY);
}

// Collision detection 2d: Aabb and Circle
inline bool Cd2AC(const Aabb2 &a, const Circle2 &c) {
	// Distance of closest point on aabb to circle
	float cx = c.x < a.minX ? a.minX : c.x;
	if (cx > a.maxX)
		cx = a.maxX;
	float cy = c.y < a.minY ? a.minY : c.y;
	if (cy > a.maxY)
		cy = a.maxY;
	float dx = cx - c.x, dy = cy - c.y;
	return dx * dx + dy * dy <= c.r * c.r;
}

// Collision detection 2d: Aabb and Line Segment
inline bool Cd2ALs(const Aabb2 &a, const LineSeg2 &ls) {
	// Based on http://www.gamedev.net/topic/338987-aabb---line-segment-intersection-test/ with bugfix.
	float hlx = (ls.x2 - ls.x1) * .5f;
	float hly = (ls.y2 - ls.y1) * .5f;
	float hax = (a.maxX - a.minX) * .5f;
	float hay = (a.maxY - a.minY) * .5f;
	float cx = fabs((ls.x1 + hlx) - (a.minX + hax));
	float cy = fabs((ls.y1 + hly) - (a.minY + hay));
	float adx = fabs(hlx);
	float ady = fabs(hly);
	if (cx > hax + adx)  // Check projection onto x axis
		return false;
	if (cy > hay + ady)  // Check projection onto y axis
		return false;
	if (adx * cy + ady * cx > ady * hax + adx * hay + .00001f)  // Check proj. onto axis perp. to line
		return false;
	return true;
}

// Collision detection 2d: Aabb and Triangle
inline bool Cd2AT(const Aabb2 &a, const Triangle2 &t) {
	const float BIG_FLT = 1E+37f;
	float minTx = t.x1, maxTx = t.x1, minTy = t.y1, maxTy = t.y1;
	if (t.x2 < minTx) minTx = t.x2; if (t.x3 < minTx) minTx = t.x3;
	if (t.x2 > maxTx) maxTx = t.x2; if (t.x3 > maxTx) maxTx = t.x3;
	if (t.y2 < minTy) minTy = t.y2; if (t.y3 < minTy) minTy = t.y3;
	if (t.y2 > maxTy) maxTy = t.y2; if (t.y3 > maxTy) maxTy = t.y3;
	if (a.minX > maxTx || a.maxX < minTx || a.minY > maxTy || a.maxY < minTy)
		return false;

	const float pvx[7] = { t.x1, t.x2, t.x3, a.minX, a.minX, a.maxX, a.maxX };
	const float pvy[7] = { t.y1, t.y2, t.y3, a.minY, a.maxY, a.minY, a.maxY };
	const int i[7] = { 0, 0, 0, 1, 1, 1, 1 };
	// Test for separating axis on all edges of triangle ONLY. We tested against aabb above.
	int lastJ = 2;
	for (int j = 0; j < 3; j++) {
		// Get vector of normal of our current test edge 
		// (note because we're comparing, normalizing vector is unneeded)
		float vecx = pvy[lastJ] - pvy[j];
		float vecy = pvx[j] - pvx[lastJ];

		// Now project both polygons onto vector, test for overlap
		float mn[2] = { BIG_FLT, BIG_FLT }, mx[2] = { -BIG_FLT, -BIG_FLT };
		for (int l = 0; l < 7; l++) {
			float d = pvx[l] * vecx + pvy[l] * vecy;
			if (d < mn[i[l]])
				mn[i[l]] = d;
			if (d > mx[i[l]])
				mx[i[l]] = d;
		}
		if (mn[0] > mx[1] || mx[0] < mn[1])
			return false;
		lastJ = j;
	}
	return true;
}

// Collision detection 2d: Aabb and Obb
inline bool Cd2AO(const Aabb2 &a, const Obb2 &o) {
	// Test obb against aabb axes
	float halfW = fabs(o.halfW * o.orientX) + fabs(o.halfH * o.orientY);
	float halfH = fabs(o.halfH * o.orientX) + fabs(o.halfW * o.orientY);
	if (a.minX > o.cx + halfW || a.maxX < o.cx - halfW || a.minY > o.cy + halfH || a.maxY < o.cy - halfH)
		return false;

	// Test aabb against obb axes
	float cx1 = 0, cy1 = 0, cx2 = 0, cy2 = 0;
	halfW = (a.maxX - a.minX) * .5f;
	halfH = (a.maxY - a.minY) * .5f;
	Project2(a.minX + halfW, a.minY + halfH, o.orientX, -o.orientY, &cx1, &cy1);
	Project2(o.cx, o.cy, o.orientX, -o.orientY, &cx2, &cy2);
	float ahalfW = fabs(halfW * o.orientX) + fabs(halfH * o.orientY);
	float ahalfH = fabs(halfH * o.orientX) + fabs(halfW * o.orientY);
	halfW = fabs(o.halfW);
	halfH = fabs(o.halfH);
	if (cx2 - halfW > cx1 + ahalfW || cx2 + halfW < cx1 - ahalfW || cy2 - halfH > cy1 + ahalfH || cy2 + halfH < cy1 - ahalfH)
		return false;

	return true;
}

// Collision detection 2d: Circle and Circle
inline bool Cd2CC(const Circle2 &c1, const Circle2 &c2) {
	float dx = c1.x - c2.x;
	float dy = c1.y - c2.y;
	float dr = fabs(c1.r) + fabs(c2.r);
	return dx * dx + dy * dy <= dr * dr;
}

// Collision detection 2d: Circle and Aabb
inline bool Cd2CA(const Circle2 &c, const Aabb2 &a) { return Cd2AC(a, c); }

// Collision detection 2d: Circle and Line Segment
inline bool Cd2CLs(Point2 *pOut, const Circle2 &c, const LineSeg2 &ls) {
	// http://www.allegro.cc/forums/thread/595918
	float A = c.x - ls.x1, B = c.y - ls.y1;
	float C = ls.x2 - ls.x1, D = ls.y2 - ls.y1;
	float dot = A * C + B * D;
	float len_sq = C * C + D * D;
	float alpha = 0;
	if (len_sq > .0000001) {
		alpha = dot / len_sq;
		// 'param' tells us which point to pick
		// If it is outside 0 to 1 range, we pick one of the endpoints, otherwise we pick a point inside the line segment
		if (alpha > 1) {
			A = c.x - ls.x2;
			B = c.y - ls.y2;
		}
		else if (alpha >= 0) {
			// circle point minus closest point on line...
			A -= alpha * C;
			B -= alpha * D;
		}
	}
	if (A * A + B * B <= c.r * c.r) {
		if (pOut) {
			pOut->x = ls.x1 + C * alpha;
			pOut->y = ls.y1 + D * alpha;
		}
		return true;
	}
	return false;
}

// Collision detection 2d: Circle and Point
inline bool Cd2CP(const Circle2 &c, const Point2 &p) {
	float dx = c.x - p.x, dy = c.y - p.y;
	return (dx * dx + dy * dy <= c.r * c.r);
}

// Collision detection 2d: Circle and Triangle
inline bool Cd2CT(const Circle2 &c, const Triangle2 &tri) {
	// See http://www.phatcode.net/articles.php?id=459 for improvement ideas.
	// This divides the collision detection into separate tests for different cases, instead of
	// using the SAT directly. Works just as well either way.
	bool b1 = (c.x - tri.x1) * (tri.y2 - tri.y1) - (c.y - tri.y1) * (tri.x2 - tri.x1) > 0;
	bool b2 = (c.x - tri.x2) * (tri.y3 - tri.y2) - (c.y - tri.y2) * (tri.x3 - tri.x2) > 0;
	bool b3 = (c.x - tri.x3) * (tri.y1 - tri.y3) - (c.y - tri.y3) * (tri.x1 - tri.x3) > 0;
	if ((b1 == b2) && (b2 == b3))
		return true;
	if (Cd2CLs(0, c, LineSeg2(tri.x1, tri.y1, tri.x2, tri.y2)) ||
		Cd2CLs(0, c, LineSeg2(tri.x2, tri.y2, tri.x3, tri.y3)) ||
		Cd2CLs(0, c, LineSeg2(tri.x3, tri.y3, tri.x1, tri.y1)))
		return true;
	return false;
}

// Collision detection 2d: Circle and Obb
inline bool Cd2CO(const Circle2 &c, const Obb2 &o) {
	float px = 0, py = 0, ox = 0, oy = 0;
	float hw = fabs(o.halfW), hh = fabs(o.halfH);
	Project2(c.x, c.y, o.orientX, o.orientY, &px, &py);
	Project2(o.cx, o.cy, o.orientX, o.orientY, &ox, &oy);
	return Cd2CA(Circle2(px, py, c.r), Aabb2(ox - hw, oy - hh, ox + hw, oy + hh));
}

// Collision detection 2d: Point and Point
inline bool Cd2PP(const Point2 &p1, const Point2 &p2) { return p1.x == p2.x && p1.y == p2.y; }

// Collision detection 2d: Point and Circle
inline bool Cd2PC(const Point2 &p, const Circle2 &c) { return Cd2CP(c, p); }

// Collision detection 2d: Point and Circle Array
// Returns -1 if none...
inline int Cd2PCv(const Point2 &p, const Circle2 *vecCircles, size_t count) {
	for (size_t i = 0; i < count; i++)
		if (Cd2CP(vecCircles[i], p))
			return static_cast<int>(i);
	return -1;
}

// Collision detection 2d: Point and Aabb
inline bool Cd2PA(const Point2 &p, const Aabb2 &a) { return Cd2AP(a, p); }

// Collision detection 2d: Point and Aabb Array
// Returns -1 if none...
inline int Cd2PAv(const Point2 &p, const Aabb2 *vecAabbs, size_t count) {
	for (size_t i = 0; i < count; i++)
		if (Cd2AP(vecAabbs[i], p))
			return static_cast<int>(i);
	return -1;
}

// Collision detection 2d: Point and Line Segment
inline bool Cd2PLs(const Point2 &p, const LineSeg2 &ls) {
	// Test if line is flat horizontally
	float adx = fabs(ls.x2 - ls.x1);
	float adxp1 = fabs(p.x - ls.x1);
	float sdy = p.y - ls.y1;
	if (adx < .00001) {
		float ady = fabs(ls.y2 - ls.y1);
		return adxp1 < .00001 && fabs(p.y - ls.y2) <= ady && fabs(sdy) <= ady;
	}
	// Test if point is outside of x range
	if (fabs(p.x - ls.x2) > adx || adxp1 > adx)
		return false;
	// Calc y on line based upon x, check if that's where the point is
	float dy = (ls.y2 - ls.y1) / (ls.x2 - ls.x1);
	return fabs((p.x - ls.x1) * dy - sdy) < .00001;
}

// Collision detection 2d: Point and Triangle
inline bool Cd2PT(const Point2 &p, const Triangle2 &tri) {
	// Triangle with 0 area will always return true when it is likely false
	if (tri.x1 == tri.x2 && tri.x2 == tri.x3 && tri.y1 == tri.y2 && tri.y2 == tri.y3)
		return p.x == tri.x1 && p.y == tri.y1;

	// Note - these are all > 0 if counter clockwise... but the return condition allows
	// for either ordering.
	bool b1 = (p.x - tri.x1) * (tri.y2 - tri.y1) - (p.y - tri.y1) * (tri.x2 - tri.x1) > 0;
	bool b2 = (p.x - tri.x2) * (tri.y3 - tri.y2) - (p.y - tri.y2) * (tri.x3 - tri.x2) > 0;
	bool b3 = (p.x - tri.x3) * (tri.y1 - tri.y3) - (p.y - tri.y3) * (tri.x1 - tri.x3) > 0;
	return ((b1 == b2) && (b2 == b3));
}

// Collision detection 2d: Point and Obb
inline bool Cd2PO(const Point2 &p, const Obb2 &o) {
	float px, py;
	Unproject2(p.x - o.cx, p.y - o.cy, o.orientX, o.orientY, &px, &py);
	float hw = fabs(o.halfW);
	float hh = fabs(o.halfH);
	return (px >= -hw && px <= hw && py >= -hh && py <= hh);
}

// Collision detection 2d: Obb and Point
inline bool Cd2OP(const Obb2 &o, const Point2 &p) { return Cd2PO(p, o); }

// Collision detection 2d: Obb and Line Segment
inline bool Cd2OLs(const Obb2 &o, const LineSeg2 &ls) {
	// Unproject line seg from local obb space (same as using Unproject2)
	LineSeg2 ls2(ls.x1 * o.orientX + ls.y1 * o.orientY, ls.y1 * o.orientX - ls.x1 * o.orientY,
		ls.x2 * o.orientX + ls.y2 * o.orientY, ls.y2 * o.orientX - ls.x2 * o.orientY);
	float cx = 0, cy = 0;
	float hw = fabs(o.halfW), hh = fabs(o.halfH);
	Unproject2(o.cx, o.cy, o.orientX, o.orientY, &cx, &cy);
	Aabb2 a(cx - hw, cy - hh, cx + hw, cy + hh);
	return Cd2ALs(a, ls2);
}

// Collision detection 2d: Obb and Circle
inline bool Cd2OC(const Obb2 &o, const Circle2 &c) { return Cd2CO(c, o); }

// Collision detection 2d: Obb and Aabb
inline bool Cd2OA(const Obb2 &o, const Aabb2 &a) { return Cd2AO(a, o); }

// Collision detection 2d: Obb and Obb
inline bool Cd2OO(const Obb2 &o1, const Obb2 &o2) {
	// @@ Pretty efficient, but there's still some redundancy
	float cx1 = 0, cy1 = 0, cx2 = 0, cy2 = 0;
	float ox = 0, oy = 0;
	// Transform to o1 local space - note that these actually use Project2 to unproject
	Project2(o1.cx, o1.cy, o1.orientX, -o1.orientY, &cx1, &cy1);
	Project2(o2.cx, o2.cy, o1.orientX, -o1.orientY, &cx2, &cy2);
	Project2(o2.orientX, o2.orientY, o1.orientX, -o1.orientY, &ox, &oy);
	float hw = fabs(o1.halfW);
	float hh = fabs(o1.halfH);
	float halfW = fabs(o2.halfW * ox) + fabs(o2.halfH * oy);
	float halfH = fabs(o2.halfH * ox) + fabs(o2.halfW * oy);
	if (cx1 - hw > cx2 + halfW || cx1 + hw < cx2 - halfW || cy1 - hh > cy2 + halfH || cy1 + hh < cy2 - halfH)
		return false;
	// Transform to o2 local space
	Project2(o1.cx, o1.cy, o2.orientX, -o2.orientY, &cx1, &cy1);
	Project2(o2.cx, o2.cy, o2.orientX, -o2.orientY, &cx2, &cy2);
	Project2(o1.orientX, o1.orientY, o2.orientX, -o2.orientY, &ox, &oy);
	hw = fabs(o2.halfW);
	hh = fabs(o2.halfH);
	halfW = fabs(o1.halfW * ox) + fabs(o1.halfH * oy);
	halfH = fabs(o1.halfH * ox) + fabs(o1.halfW * oy);
	if (cx2 - hw > cx1 + halfW || cx2 + hw < cx1 - halfW || cy2 - hh > cy1 + halfH || cy2 + hh < cy1 - halfH)
		return false;

	return true;
}

// Collision detection 2d: Obb and Triangle
inline bool Cd2OT(const Obb2 &o, const Triangle2 &t) {
	// Naive impl.
	float cx = 0, cy = 0;
	float hw = fabs(o.halfW), hh = fabs(o.halfH);
	Unproject2(o.cx, o.cy, o.orientX, o.orientY, &cx, &cy);
	Aabb2 a(cx - hw, cy - hh, cx + hw, cy + hh);
	Triangle2 t2;
	Unproject2(t.x1, t.y1, o.orientX, o.orientY, &t2.x1, &t2.y1);
	Unproject2(t.x2, t.y2, o.orientX, o.orientY, &t2.x2, &t2.y2);
	Unproject2(t.x3, t.y3, o.orientX, o.orientY, &t2.x3, &t2.y3);
	return Cd2AT(a, t2);
}

// Collision detection 2d: Line Segment and Line Segment
inline bool Cd2LsLs(Point2 *pOut, const LineSeg2 &ls1, const LineSeg2 &ls2) {
	float ls2x = ls2.x2 - ls2.x1;
	float ls2y = ls2.y2 - ls2.y1;
	float ls1x = ls1.x2 - ls1.x1;
	float ls1y = ls1.y2 - ls1.y1;
	float lsy1 = ls1.y1 - ls2.y1;
	float lsx1 = ls1.x1 - ls2.x1;
	float uan = ls2x * lsy1 - ls2y * lsx1;
	float den = ls2y * ls1x - ls2x * ls1y;
	float ubn = ls1x * lsy1 - ls1y * lsx1;
	// Lines are collinear
	if (den == 0 && uan == 0 && ubn == 0) {
		float minx1 = ls1.x1 < ls1.x2 ? ls1.x1 : ls1.x2;
		float maxx1 = ls1.x1 < ls1.x2 ? ls1.x2 : ls1.x1;
		float minx2 = ls2.x1 < ls2.x2 ? ls2.x1 : ls2.x2;
		float maxx2 = ls2.x1 < ls2.x2 ? ls2.x2 : ls2.x1;
		float miny1 = ls1.y1 < ls1.y2 ? ls1.y1 : ls1.y2;
		float maxy1 = ls1.y1 < ls1.y2 ? ls1.y2 : ls1.y1;
		float miny2 = ls2.y1 < ls2.y2 ? ls2.y1 : ls2.y2;
		float maxy2 = ls2.y1 < ls2.y2 ? ls2.y2 : ls2.y1;
		if (!(minx1 <= maxx2 && maxx1 >= minx2 && miny1 <= maxy2 && maxy1 >= miny2))
			return false;
		if (pOut) {
			pOut->x = ls1.x1;
			pOut->y = ls1.y1;
		}
		return true;
	}
	else if (den != 0) {  // parallel lines if den == 0
		float iden = 1.0f / den;
		// Line segment intersection...
		if (uan * iden >= 0 && uan * iden <= 1 && ubn * iden >= 0 && ubn * iden <= 1) {
			if (pOut) {
				pOut->x = ls1.x1 + (ls1.x2 - ls1.x1) * uan * iden;
				pOut->y = ls1.y1 + (ls1.y2 - ls1.y1) * uan * iden;
			}
			return true;
		}
	}
	return false;
}

// Collision detection 2d: Line Segment and Aabb
inline bool Cd2LsA(const LineSeg2 &ls, const Aabb2 &a) { return Cd2ALs(a, ls); }

// Collision detection 2d: Line Segment and Circle
inline bool Cd2LsC(Point2 *pOut, const LineSeg2 &ls, const Circle2 &c) { return Cd2CLs(pOut, c, ls); }

// Collision detection 2d: Point and Line Segment
inline bool Cd2LsP(const LineSeg2 &ls, const Point2 &p) { return Cd2PLs(p, ls); }

// Collision detection 2d: Line Segment and Triangle
inline bool Cd2LsT(const LineSeg2 &ls, const Triangle2 &tri) {
	// Naive impl.
	if (Cd2PT(Point2(ls.x1, ls.y1), tri) || Cd2PT(Point2(ls.x2, ls.y2), tri))
		return true;
	if (Cd2LsLs(0, LineSeg2(tri.x1, tri.y1, tri.x2, tri.y2), ls) || 
		Cd2LsLs(0, LineSeg2(tri.x2, tri.y2, tri.x3, tri.y3), ls) || 
		Cd2LsLs(0, LineSeg2(tri.x3, tri.y3, tri.x1, tri.y1), ls))
		return true;
	return false;
}

// Collision detection 2d: Line Segment and Obb
inline bool Cd2LsO(const LineSeg2 &ls, const Obb2 &o) { return Cd2OLs(o, ls); }

// Collision detection 2d: Triangle and Line Segment
inline bool Cd2TLs(const Triangle2 &tri, const LineSeg2 &ls) { return Cd2LsT(ls, tri); }

// Collision detection 2d: Triangle and Point
inline bool Cd2TP(const Triangle2 &tri, const Point2 &p) { return Cd2PT(p, tri); }

// Collision detection 2d: Triangle and Circle
inline bool Cd2TC(const Triangle2 &tri, const Circle2 &c) { return Cd2CT(c, tri); }

// Collision detection 2d: Aabb and Triangle
inline bool Cd2TA(const Triangle2 &t, const Aabb2 &a) { return Cd2AT(a, t); }

// Collision detection 2d: Triangle and Triangle
inline bool Cd2TT(const Triangle2 &t1, const Triangle2 &t2) {
	float t1x[3] = { t1.x1, t1.x2, t1.x3 };
	float t1y[3] = { t1.y1, t1.y2, t1.y3 };
	float t2x[3] = { t2.x1, t2.x2, t2.x3 };
	float t2y[3] = { t2.y1, t2.y2, t2.y3 };
	return Cd2NN(t1x, t1y, 3, t2x, t2y, 3);
}

// Collision detection 2d: Triangle and Obb
inline bool Cd2TO(const Triangle2 &t, const Obb2 &o) { return Cd2OT(o, t); }

//-----------------------------------------------------------------------------------------------------------
// DISTANCE SQUARED CALCULATION
//-----------------------------------------------------------------------------------------------------------

// Distance 2d squared: Point and Point
inline float Dist2PP(Point2 p1, Point2 p2) {
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	return dx * dx + dy * dy;
}

// Distance 2d squared: Point and Line Segment
inline float Dist2PLs(Point2 p, LineSeg2 ls) {
	float line2 = (ls.x1 - ls.x2) * (ls.x1 - ls.x2) + (ls.y1 - ls.y2) * (ls.y1 - ls.y2);
	if (line2 == 0)
		return (p.x - ls.x1) * (p.x - ls.x1) + (p.y - ls.y1) * (p.y - ls.y1);
	float t = ((p.x - ls.x1) * (ls.x2 - ls.x1) + (p.y - ls.y1) * (ls.y2 - ls.y1)) / line2;
	if (t < 0)
		return (p.x - ls.x1) * (p.x - ls.x1) + (p.y - ls.y1) * (p.y - ls.y1);
	else if (t > 1)
		return (p.x - ls.x2) * (p.x - ls.x2) + (p.y - ls.y2) * (p.y - ls.y2);
	float xd = ls.x1 + t * (ls.x2 - ls.x1);
	float yd = ls.y1 + t * (ls.y2 - ls.y1);
	return (p.x - xd) * (p.x - xd) + (p.y - yd) * (p.y - yd);
}

// Distance 2d squared: Point and Circle
inline float Dist2PC(Point2 p, Circle2 c) {
	float dx = c.x - p.x;
	float dy = c.y - p.y;
	return dx * dx + dy * dy - c.r * c.r;
}

// Distance 2d squared: Circle and Circle
inline float Dist2CC(Circle2 c1, Circle2 c2) {
	float dx = c1.x - c2.x;
	float dy = c1.y - c2.y;
	return dx * dx + dy * dy - c1.r * c1.r - c2.r * c2.r;
}

// Distance 2d squared: Circle and Point
inline float Dist2CP(Circle2 c, Point2 p) { return Dist2PC(p, c); }
	
// Distance 2d squared: Line Segment and Point
inline float Dist2LsP(LineSeg2 ls, Point2 p) { return Dist2PLs(p, ls); }

}  // namespace

#endif  // _SAW_GEOM_CD2_INCLUDED
