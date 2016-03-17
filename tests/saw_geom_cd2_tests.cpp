#include <iostream>
#include "saw_geom_cd2.h"

using std::cout;

int main() {
#define TESTAA(x1, x2, y1, y2, x3, x4, y3, y4, v, i) \
	if (Cd2AA(Aabb2(x1, y1, x2, y2), Aabb2(x3, y3, x4, y4)) != v) \
		cout << "Failed Cd2AA in test " << i << ".\r\n"; \
	else if (Cd2AA(Aabb2(x3, y3, x4, y4), Aabb2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2AA reversal (Cd2AA) in test " << i << ".\r\n"
#define TESTAA4(x1, x2, y1, y2, x3, x4, y3, y4, v, i) \
	TESTAA(x1, x2, y1, y2, x3, x4, y3, y4, v, i); TESTAA(-x2, -x1, y1, y2, -x4, -x3, y3, y4, v, i + 1); \
	TESTAA(x1, x2, -y2, -y1, x3, x4, -y4, -y3, v, i + 2); TESTAA(-x2, -x1, -y2, -y1, -x4, -x3, -y4, -y3, v, i + 3);
	TESTAA4(10, 15, 10, 15, 11, 14, 5, 9, false, 0);  // b above a
	TESTAA4(10, 15, 10, 15, 11, 14, 9, 11, true, 4);
	TESTAA4(10, 15, 10, 15, 11, 14, 16, 20, false, 8);  // b below a
	TESTAA4(10, 15, 10, 15, 11, 14, 14, 16, true, 12);
	TESTAA4(10, 15, 10, 15, 5, 9, 11, 14, false, 16);  // b left of a
	TESTAA4(10, 15, 10, 15, 9, 11, 11, 14, true, 20);
	TESTAA4(10, 15, 10, 15, 16, 20, 11, 14, false, 24);  // b right of a
	TESTAA4(10, 15, 10, 15, 14, 16, 11, 14, true, 28);
	TESTAA4(10, 15, 10, 15, 5, 9, 5, 9, false, 32);  // b up-left of a
	TESTAA4(10, 15, 10, 15, 9, 11, 9, 11, true, 36);
	TESTAA4(10, 15, 10, 15, 16, 20, 5, 9, false, 40);  // b up-right of a
	TESTAA4(10, 15, 10, 15, 14, 16, 9, 11, true, 44);
	TESTAA4(10, 15, 10, 15, 5, 9, 16, 20, false, 48);  // b down-left of a
	TESTAA4(10, 15, 10, 15, 9, 11, 14, 16, true, 52);
	TESTAA4(10, 15, 10, 15, 16, 20, 16, 20, false, 56);  // b down-right of a
	TESTAA4(10, 15, 10, 15, 14, 16, 14, 16, true, 60);
	TESTAA4(10, 15, 10, 15, 11, 14, 11, 14, true, 64);  // b inside a
	TESTAA4(11, 14, 11, 14, 10, 15, 10, 15, true, 68);  // a inside b

#define TESTAP(x1, x2, y1, y2, x3, y3, v, i) \
	if (Cd2AP(Aabb2(x1, y1, x2, y2), Point2(x3, y3)) != v) \
		cout << "Failed Cd2AP in test " << i << ".\r\n"; \
	if (Cd2PA(Point2(x3, y3), Aabb2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2AP reversal (Cd2PA) in test " << i << ".\r\n"
#define TESTAP4(x1, x2, y1, y2, x3, y3, v, i) \
	TESTAP(x1, x2, y1, y2, x3, y3, v, i); TESTAP(-x2, -x1, y1, y2, -x3, y3, v, i + 1); \
	TESTAP(x1, x2, -y2, -y1, x3, -y3, v, i + 2); TESTAP(-x2, -x1, -y2, -y1, -x3, -y3, v, i + 3);
	TESTAP4(10, 15, 10, 15, 12, 9, false, 0);  // b above a
	TESTAP4(10, 15, 10, 15, 12, 16, false, 4);  // b below a
	TESTAP4(10, 15, 10, 15, 9, 12, false, 8);  // b left of a
	TESTAP4(10, 15, 10, 15, 16, 12, false, 12);  // b right of a
	TESTAP4(10, 15, 10, 15, 9, 9, false, 16);  // b up-left of a
	TESTAP4(10, 15, 10, 15, 16, 9, false, 20);  // b up-right of a
	TESTAP4(10, 15, 10, 15, 9, 16, false, 24);  // b down-left of a
	TESTAP4(10, 15, 10, 15, 16, 16, false, 28);  // b down-right of a
	TESTAP4(10, 15, 10, 15, 14, 14, true, 32);  // b inside a

#define TESTAC(x1, x2, y1, y2, x3, y3, r, v, i) \
	if (Cd2AC(Aabb2(x1, y1, x2, y2), Circle2(x3, y3, r)) != v) \
		cout << "Failed Cd2AC in test " << i << ".\r\n"; \
	if (Cd2CA(Circle2(x3, y3, r), Aabb2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2AC reversal (Cd2CA) in test " << i << ".\r\n"
#define TESTAC4(x1, x2, y1, y2, x3, y3, r, v, i) \
	TESTAC(x1, x2, y1, y2, x3, y3, r, v, i); TESTAC(-x2, -x1, y1, y2, -x3, y3, r, v, i + 1); \
	TESTAC(x1, x2, -y2, -y1, x3, -y3, r, v, i + 2); TESTAC(-x2, -x1, -y2, -y1, -x3, -y3, r, v, i + 3);
	TESTAC4(10, 15, 10, 15, 12, 9, .5f, false, 0);  // b above a
	TESTAC4(10, 15, 10, 15, 12, 9, 2.0f, true, 4);
	TESTAC4(10, 15, 10, 15, 12, 16, .5f, false, 8);  // b below a
	TESTAC4(10, 15, 10, 15, 12, 16, 2.0f, true, 12);
	TESTAC4(10, 15, 10, 15, 9, 12, .5f, false, 16);  // b left of a
	TESTAC4(10, 15, 10, 15, 9, 12, 2.0f, true, 20);
	TESTAC4(10, 15, 10, 15, 16, 12, .5f, false, 24);  // b right of a
	TESTAC4(10, 15, 10, 15, 16, 12, 2.0f, true, 28);
	TESTAC4(10, 15, 10, 15, 9, 9, .5f, false, 32);  // b up-left of a
	TESTAC4(10, 15, 10, 15, 9, 9, 2.0f, true, 36);
	TESTAC4(10, 15, 10, 15, 16, 9, .5f, false, 40);  // b up-right of a
	TESTAC4(10, 15, 10, 15, 16, 9, 2.0f, true, 44);
	TESTAC4(10, 15, 10, 15, 9, 16, .5f, false, 48);  // b down-left of a
	TESTAC4(10, 15, 10, 15, 9, 16, 2.0f, true, 52);
	TESTAC4(10, 15, 10, 15, 16, 16, .5f, false, 56);  // b down-right of a
	TESTAC4(10, 15, 10, 15, 16, 16, 2.0f, true, 60);
	TESTAC4(10, 15, 10, 15, 12, 12, 1.0f, true, 64);  // b inside a
	TESTAC4(11, 14, 11, 14, 12, 12, 7.0f, true, 68);  // a inside b

#define TESTAL(x1, x2, y1, y2, x3, x4, y3, y4, v, i) \
	if (Cd2ALs(Aabb2(x1, y1, x2, y2), LineSeg2(x3, y3, x4, y4)) != v) \
		cout << "Failed Cd2ALs in test " << i << ".\r\n"; \
	if (Cd2LsA(LineSeg2(x3, y3, x4, y4), Aabb2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2ALs reversal (Cd2LsA) in test " << i << ".\r\n"
#define TESTAL4(x1, x2, y1, y2, x3, x4, y3, y4, v, i) \
	TESTAL(x1, x2, y1, y2, x3, x4, y3, y4, v, i); TESTAL(-x2, -x1, y1, y2, -x4, -x3, y3, y4, v, i + 1); \
	TESTAL(x1, x2, -y2, -y1, x3, x4, -y4, -y3, v, i + 2); TESTAL(-x2, -x1, -y2, -y1, -x4, -x3, -y4, -y3, v, i + 3);
	TESTAL4(10, 15, 10, 15, 11, 14, 5, 9, false, 0);  // b above a
	TESTAL4(10, 15, 10, 15, 11, 14, 9, 11, true, 4);
	TESTAL4(10, 15, 10, 15, 11, 14, 16, 20, false, 8);  // b below a
	TESTAL4(10, 15, 10, 15, 11, 14, 14, 16, true, 12);
	TESTAL4(10, 15, 10, 15, 5, 9, 11, 14, false, 16);  // b left of a
	TESTAL4(10, 15, 10, 15, 9, 11, 11, 14, true, 20);
	TESTAL4(10, 15, 10, 15, 16, 20, 11, 14, false, 24);  // b right of a
	TESTAL4(10, 15, 10, 15, 14, 16, 11, 14, true, 28);
	TESTAL4(10, 15, 10, 15, 5, 9, 9, 5, false, 32);  // b up-left of a
	TESTAL4(10, 15, 10, 15, 5, 11, 11, 5, false, 36);  // b up-left of a
	TESTAL4(10, 15, 10, 15, 9, 12, 9, 11, true, 40);
	TESTAL4(10, 15, 10, 15, 16, 20, 5, 9, false, 44);  // b up-right of a
	TESTAL4(10, 15, 10, 15, 14, 20, 5, 11, false, 48);  // b up-right of a
	TESTAL4(10, 15, 10, 15, 13, 16, 9, 11, true, 52);
	TESTAL4(10, 15, 10, 15, 5, 9, 16, 20, false, 56);  // b down-left of a
	TESTAL4(10, 15, 10, 15, 5, 11, 14, 20, false, 60);  // b down-left of a
	TESTAL4(10, 15, 10, 15, 9, 12, 14, 16, true, 64);
	TESTAL4(10, 15, 10, 15, 16, 20, 16, 20, false, 68);  // b down-right of a
	TESTAL4(10, 15, 10, 15, 14, 20, 20, 14, false, 72);  // b down-right of a
	TESTAL4(10, 15, 10, 15, 13, 16, 14, 16, true, 76);
	TESTAL4(10, 15, 10, 15, 11, 14, 11, 14, true, 80);  // b inside a
	TESTAL4(11, 14, 11, 14, 10, 15, 10, 15, true, 84);  // a inside b

#define TESTAT(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i) \
	if (Cd2AT(Aabb2(x1, y1, x2, y2), Triangle2(x3, y3, x4, y4, x5, y5)) != v) \
		cout << "Failed Cd2AT in test " << i << ".\r\n"; \
	if (Cd2TA(Triangle2(x3, y3, x4, y4, x5, y5), Aabb2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2AT reversal (Cd2TA) in test " << i << ".\r\n"
#define TESTAT4(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i) \
	TESTAT(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i); TESTAT(-(x2), y1, -(x1), y2, -(x3), y3, -(x4), y4, -(x5), y5, v, i + 1); \
	TESTAT(x1, -(y2), x2, -(y1), x3, -(y3), x4, -(y4), x5, -(y5), v, i + 2); TESTAT(-(x2), -(y2), -(x1), -(y1), -(x3), -(y3), -(x4), -(y4), -(x5), -(y5), v, i + 3);
	// Test with tri verts going ccw and cw
#define TESTAT4B(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i) \
	TESTAT4(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i) \
	TESTAT4(x1, y1, x2, y2, x5, y5, x4, y4, x3, y3, v, i + 4)
	// SAT on triangle axes
	TESTAT4B(2, 2, 6, 6, -2, -2, 3, 0, 0, 3, false, 0);
	TESTAT4B(2, 2, 6, 6, 5, -2, 10, 0, 7, 3, false, 8);
	TESTAT4B(2, 2, 6, 6, -2, 5, 3, 7, 0, 10, false, 16);
	TESTAT4B(2, 2, 6, 6, 8, 5, 10, 10, 5, 8, false, 24);
	// intersections
	TESTAT4B(2, 2, 6, 6, 5, 5, 10, 7, 7, 10, true, 32);  // points from both intersect the other
	TESTAT4B(2, 2, 6, 6, 7, 4, 9, 9, 4, 7, true, 40);  // aabb point intersects triangle edge
	TESTAT4B(2, 2, 6, 6, 5.5f, 5, 10.5f, 7, 7.5f, 10, true, 48);  // aabb edge intersects triangle point
	TESTAT4B(2, 2, 11, 11, 5.5f, 5, 10.5f, 7, 7.5f, 10, true, 56);  // triangle in aabb
	TESTAT4B(7, 6, 8, 7, 5.5f, 5, 10.5f, 6, 7.5f, 10, true, 64);  // aabb in triangle
	// SAT on AABB axes
	TESTAT4B(2, 2, 6, 6, -4, -2, 1, 0, -2, 3, false, 72);  // left of aabb
	TESTAT4B(2, 2, 6, 6, 5, -4, 10, -2, 7, 1, false, 80);  // above aabb
	TESTAT4B(2, 2, 6, 6, -2, 7, 3, 9, 0, 12, false, 88);  // below aabb
	TESTAT4B(2, 2, 6, 6, 7, 5, 12, 7, 9, 10, false, 96);  // right of aabb

#define TESTAO(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i) \
	if (Cd2AO(Aabb2(x1, y1, x2, y2), Obb2(cx, cy, ox, oy, hx, hy)) != v) \
		cout << "Failed Cd2AO in test " << i << ".\r\n"; \
	if (Cd2OA(Obb2(cx, cy, ox, oy, hx, hy), Aabb2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2AO reversal (Cd2OA) in test " << i << ".\r\n"
#define TESTAO4(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i) \
	TESTAO(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i); TESTAO(-(x2), y1, -(x1), y2, -(cx), cy, hx, hy, -(ox), oy, v, i + 1); \
	TESTAO(x1, -(y2), x2, -(y1), cx, -(cy), hx, hy, ox, -(oy), v, i + 2); TESTAO(-(x2), -(y2), -(x1), -(y1), -(cx), -(cy), hx, hy, -(ox), -(oy), v, i + 3);
#define TESTAO44(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i) \
	TESTAO4(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i); TESTAO4(x1, y1, x2, y2, cx, cy, -hx, hy, ox, oy, v, i + 4); \
	TESTAO4(x1, y1, x2, y2, cx, cy, hx, -hy, ox, oy, v, i + 8); TESTAO4(x1, y1, x2, y2, cx, cy, -hx, -hy, ox, oy, v, i + 12)
	// Test against aabb separating axes
	TESTAO44(2, 2, 6, 6, 4, 10.5f, 1.8f, 3.6f, .8321f, .5547f, false, 0);  // aabb above obb
	TESTAO44(2, 2, 6, 6, 4, -2.5f, 1.8f, 3.6f, .8321f, .5547f, false, 16);  // aabb below obb
	TESTAO44(2, 2, 6, 6, -2, 4, 1.8f, 3.6f, .8321f, .5547f, false, 32);  // aabb right of obb
	TESTAO44(2, 2, 6, 6, 10, 4, 1.8f, 3.6f, .8321f, .5547f, false, 48);  // aabb left of obb
	// Test against obb separating axes
	TESTAO44(2, 2, 6, 6, .5f, .5f, 1.8f, 3.6f, .8321f, .5547f, false, 64);  // obb ul of aabb
	TESTAO44(2, 2, 6, 6, 7.5f, 7.5f, 1.8f, 3.6f, .8321f, .5547f, false, 80);  // obb br of aabb
	TESTAO44(2, 2, 6, 6, 8.5f, -1.5f, 1.8f, 3.6f, .8321f, .5547f, false, 96);  // obb ur of aabb
	TESTAO44(2, 2, 6, 6, -.5f, 9.5f, 1.8f, 3.6f, .8321f, .5547f, false, 112);  // obb bl of aabb
	// Intersection
	TESTAO44(2, 2, 6, 6, 1.5f, 8, 1.8f, 3.6f, .8321f, .5547f, true, 128);  // obb bl of aabb (aabb point crosses obb edge)
	TESTAO44(2, 2, 6, 6, 1, 8, 1.8f, 3.6f, .8321f, .5547f, true, 144);  // obb bl of aabb (obb point croses aabb edge)
	TESTAO44(2, 2, 6, 6, 4, 4, 1, .5f, .8321f, .5547f, true, 160);  // obb in aabb
	TESTAO44(2, 2, 6, 6, 4, 4, 4, 5, .8321f, .5547f, true, 176);  // aabb in obb

#define TESTPP(x1, y1, x2, y2, v, i) \
	if (Cd2PP(Point2(x1, y1), Point2(x2, y2)) != v) \
		cout << "Failed Cd2PP in test " << i << ".\r\n"; \
	if (Cd2PP(Point2(x2, y2), Point2(x1, y1)) != v) \
		cout << "Failed Cd2PP reversal (Cd2PP) in test " << i << ".\r\n"
#define TESTPP4(x1, y1, x2, y2, v, i) \
	TESTPP(x1, y1, x2, y2, v, i); TESTPP(-x1, y1, -x2, y2, v, i + 1); \
	TESTPP(x1, -y1, x2, -y2, v, i + 2); TESTPP(-x1, -y1, -x2, -y2, v, i + 3);
	TESTPP4(10.354f, 11.563f, 10.354f, 13.0f, false, 0);
	TESTPP4(10.354f, 11.563f, 14.0f, 11.563f, false, 4);
	TESTPP4(10.354f, 11.563f, 14.0f, 15.0f, false, 8);
	TESTPP4(10.354f, 11.563f, 10.354f, 11.563f, true, 12);
	TESTPP4(10.354f, 11.563f, 10.3541f, 11.5631f, false, 16);
	TESTPP4(10.35400001f, 11.563f, 10.354f, 11.56300001f, true, 20);

#define TESTPC(x1, y1, x2, y2, r, v, i) \
	if (Cd2PC(Point2(x1, y1), Circle2(x2, y2, r)) != v) \
		cout << "Failed Cd2PC in test " << i << ".\r\n"; \
	if (Cd2CP(Circle2(x2, y2, r), Point2(x1, y1)) != v) \
		cout << "Failed Cd2PC reversal (Cd2CP) in test " << i << ".\r\n"
#define TESTPC4(x1, y1, x2, y2, r, v, i) \
	TESTPC(x1, y1, x2, y2, r, v, i); TESTPC(-x1, y1, -x2, y2, r, v, i + 1); \
	TESTPC(x1, -y1, x2, -y2, r, v, i + 2); TESTPC(-x1, -y1, -x2, -y2, r, v, i + 3);
	TESTPC4(4, 4, 10, 10, 5, false, 0);
	TESTPC4(4, 4, 1, 1, 2.9f, false, 4);
	TESTPC4(4, 4, 4, 10, 6.1f, true, 8);
	TESTPC4(4, 4, 10, 10, 6.1f, false, 12);
	TESTPC4(4, 4, 1, 1, 7, true, 16);

#define TESTPL(x1, y1, x2, y2, x3, y3, v, i) \
	if (Cd2PLs(Point2(x1, y1), LineSeg2(x2, y2, x3, y3)) != v) \
		cout << "Failed Cd2PLs in test " << i << ".\r\n"; \
	if (Cd2LsP(LineSeg2(x2, y2, x3, y3), Point2(x1, y1)) != v) \
		cout << "Failed Cd2PLs reversal (Cd2LsP) in test " << i << ".\r\n"
#define TESTPL4(x1, y1, x2, y2, x3, y3, v, i) \
	TESTPL(x1, y1, x2, y2, x3, y3, v, i); TESTPL(-x1, y1, -x2, y2, -x3, y3, v, i + 1); \
	TESTPL(x1, -y1, x2, -y2, x3, -y3, v, i + 2); TESTPL(-x1, -y1, -x2, -y2, -x3, -y3, v, i + 3);
	TESTPL4(3, 3, 1, 2, 5, 4, true, 0);  // On line, on segment
	TESTPL4(3, 3, 0, 2, 5, 4, false, 4);  // Off line, within segment aabb
	TESTPL4(7, 5, 1, 2, 5, 4, false, 8);  // On line, but off segment (aka off line, outside aabb)
	TESTPL4(4, 4, 1, 4, 7, 4, true, 12);
	TESTPL4(4, 4, 5, 4, 7, 4, false, 16);
	TESTPL4(4, 4, 4, 1, 4, 7, true, 20);
	TESTPL4(4, 4, 4, 5, 4, 7, false, 24);

#define TESTPT(x1, y1, x2, y2, x3, y3, x4, y4, v, i) \
	if (Cd2PT(Point2(x1, y1), Triangle2(x2, y2, x3, y3, x4, y4)) != v) \
		cout << "Failed Cd2PT in test " << i << ".\r\n"; \
	if (Cd2TP(Triangle2(x2, y2, x3, y3, x4, y4), Point2(x1, y1)) != v) \
		cout << "Failed Cd2PT reversal (Cd2TP) in test " << i << ".\r\n"
#define TESTPT4(x1, y1, x2, y2, x3, y3, x4, y4, v, i) \
	TESTPT(x1, y1, x2, y2, x3, y3, x4, y4, v, i); TESTPT(-x1, y1, -x2, y2, -x3, y3, -x4, y4, v, i + 1); \
	TESTPT(x1, -y1, x2, -y2, x3, -y3, x4, -y4, v, i + 2); TESTPT(-x1, -y1, -x2, -y2, -x3, -y3, -x4, -y4, v, i + 3);
	// Test with tri verts going ccw and cw
#define TESTPT4B(x1, y1, x2, y2, x3, y3, x4, y4, v, i) \
	TESTPT4(x1, y1, x2, y2, x3, y3, x4, y4, v, i) \
	TESTPT4(x1, y1, x4, y4, x3, y3, x2, y2, v, i + 4)
	TESTPT4B(3, 3, 3, 1, 4, 4, 2, 5, true, 0);
	TESTPT4B(1, 1.5f, 3, 1, 4, 4, 2, 5, false, 8);
	TESTPT4B(3.5f, 2, 3, 1, 4, 4, 2, 5, false, 16);
	TESTPT4B(3, 4.9f, 3, 1, 4, 4, 2, 5, false, 24);

#define TESTPO(x1, y1, cx, cy, hx, hy, ox, oy, v, i) \
	if (Cd2PO(Point2(x1, y1), Obb2(cx, cy, ox, oy, hx, hy)) != v) \
		cout << "Failed Cd2PO in test " << i << ".\r\n"; \
	if (Cd2OP(Obb2(cx, cy, ox, oy, hx, hy), Point2(x1, y1)) != v) \
		cout << "Failed Cd2PO reversal (Cd2OP) in test " << i << ".\r\n"
#define TESTPO4(x1, y1, cx, cy, hx, hy, ox, oy, v, i) \
	TESTPO(x1, y1, cx, cy, hx, hy, ox, oy, v, i); TESTPO(-(x1), y1, -(cx), cy, hx, hy, -(ox), oy, v, i + 1); \
	TESTPO(x1, -(y1), cx, -(cy), hx, hy, ox, -(oy), v, i + 2); TESTPO(-(x1), -(y1), -(cx), -(cy), hx, hy, -(ox), -(oy), v, i + 3);
#define TESTPO44(x1, y1, cx, cy, hx, hy, ox, oy, v, i) \
	TESTPO4(x1, y1, cx, cy, hx, hy, ox, oy, v, i); TESTPO4(x1, y1, cx, cy, -hx, hy, ox, oy, v, i + 4); \
	TESTPO4(x1, y1, cx, cy, hx, -hy, ox, oy, v, i + 8); TESTPO4(x1, y1, cx, cy, -hx, -hy, ox, oy, v, i + 12)
	TESTPO44(5, 5, 4, 4, 3, 4,.8321f, .5547f, true, 0);
	TESTPO44(8, 7, 4, 4, 3, 4, .8321f, .5547f, false, 16);
	TESTPO44(7, .5f, 4, 4, 3, 4, .8321f, .5547f, false, 32);
	TESTPO44(.5f, 3, 4, 4, 3, 4, .8321f, .5547f, false, 48);
	TESTPO44(2, 8, 4, 4, 3, 4, .8321f, .5547f, false, 64);

#define TESTCC(x1, y1, r1, x2, y2, r2, v, i) \
	if (Cd2CC(Circle2(x1, y1, r1), Circle2(x2, y2, r2)) != v) \
		cout << "Failed Cd2CC in test " << i << ".\r\n"; \
	if (Cd2CC(Circle2(x2, y2, r2), Circle2(x1, y1, r1)) != v) \
		cout << "Failed Cd2CC reversal (Cd2CC) in test " << i << ".\r\n"
#define TESTCC4(x1, y1, r1, x2, y2, r2, v, i) \
	TESTCC(x1, y1, r1, x2, y2, r2, v, i); TESTCC(-x1, y1, r1, -x2, y2, r2, v, i + 1); \
	TESTCC(x1, -y1, r1, x2, -y2, r2, v, i + 2); TESTCC(-x1, -y1, r1, -x2, -y2, r2, v, i + 3);
#define TESTCC44(x1, y1, r1, x2, y2, r2, v, i) \
	TESTCC4(x1, y1, r1, x2, y2, r2, v, i); TESTCC4(x1, y1, -r1, x2, y2, r2, v, i + 4); \
	TESTCC4(x1, y1, r1, x2, y2, -r2, v, i + 8); TESTCC4(x1, y1, -r1, x2, y2, -r2, v, i + 12)
	TESTCC44(10, 10, 5, 9, 9, 1, true, 0);
	TESTCC44(10, 10, 1, 9, 9, 5, true, 16);
	TESTCC44(10, 10, 5, 10, 20, 6, true, 32);
	TESTCC44(10, 10, 5, 21.1f, 10, 6, false, 48);
	TESTCC44(10, 10, 5, 10, 22.5f, 7, false, 64);

#define TESTCL(x1, x2, y1, y2, x3, y3, r, v, i) \
	if (Cd2CLs(Circle2(x3, y3, r), LineSeg2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2CLs in test " << i << ".\r\n"; \
	if (Cd2LsC(LineSeg2(x1, y1, x2, y2), Circle2(x3, y3, r)) != v) \
		cout << "Failed Cd2CLs reversal (Cd2LsC) in test " << i << ".\r\n"
#define TESTCL4(x1, x2, y1, y2, x3, y3, r, v, i) \
	TESTCL(x1, x2, y1, y2, x3, y3, r, v, i); TESTCL(-x1, -x2, y1, y2, -x3, y3, r, v, i + 1); \
	TESTCL(x1, x2, -x1, -y2, x3, -y3, r, v, i + 2); TESTCL(-x1, -x2, -y1, -y2, -x3, -y3, r, v, i + 3);
#define TESTCL42(x1, x2, y1, y2, x3, y3, r, v, i) \
	TESTCL4(x1, x2, y1, y2, x3, y3, r, v, i); TESTCL4(x1, x2, y1, y2, x3, y3, -r, v, i + 4);
	TESTCL42(10, 15, 10, 15, 12, 9, .5f, false, 0);  // b above a
	TESTCL42(10, 15, 10, 15, 12, 16, .5f, false, 8);  // b below a
	TESTCL42(10, 15, 10, 15, 9, 12, .5f, false, 16);  // b left of a
	TESTCL42(10, 15, 10, 15, 16, 12, .5f, false, 24);  // b right of a
	TESTCL42(10, 15, 10, 15, 9, 9, .5f, false, 32);  // b up-left of a
	TESTCL42(10, 15, 10, 15, 16, 9, .5f, false, 40);  // b up-right of a
	TESTCL42(10, 15, 10, 15, 9, 16, .5f, false, 48);  // b down-left of a
	TESTCL42(10, 15, 10, 15, 16, 16, .5f, false, 56);  // b down-right of a
	TESTCL42(10, 15, 10, 15, 9, 9, 2.0f, true, 64);
	TESTCL42(10, 15, 10, 15, 16, 16, 2.0f, true, 72);
	TESTCL42(10, 15, 10, 15, 12, 12, 1.0f, true, 80);  // b inside a
	TESTCL42(11, 14, 11, 14, 12, 12, 10.0f, true, 88);  // a inside b
	TESTCL42(12, 12, 12, 12, 13, 13, 1.5f, true, 96);  // Line segment 0 length
	TESTCL42(12, 12, 12, 12, 13, 13, .5f, false, 104);  // Line segment 0 length

#define TESTCT(x1, y1, r, x2, y2, x3, y3, x4, y4, v, i) \
	if (Cd2CT(Circle2(x1, y1, r), Triangle2(x2, y2, x3, y3, x4, y4)) != v) \
		cout << "Failed Cd2CT in test " << i << ".\r\n"; \
	if (Cd2TC(Triangle2(x2, y2, x3, y3, x4, y4), Circle2(x1, y1, r)) != v) \
		cout << "Failed Cd2CT reversal (Cd2TC) in test " << i << ".\r\n"
#define TESTCT4(x1, y1, r, x2, y2, x3, y3, x4, y4, v, i) \
	TESTCT(x1, y1, r, x2, y2, x3, y3, x4, y4, v, i); TESTCT(-x1, y1, r, -x2, y2, -x3, y3, -x4, y4, v, i + 1); \
	TESTCT(x1, -y1, r, x2, -y2, x3, -y3, x4, -y4, v, i + 2); TESTCT(-x1, -y1, r, -x2, -y2, -x3, -y3, -x4, -y4, v, i + 3);
	// Test with tri verts going ccw and cw
#define TESTCT4B(x1, y1, r, x2, y2, x3, y3, x4, y4, v, i) \
	TESTCT4(x1, y1, r, x2, y2, x3, y3, x4, y4, v, i) \
	TESTCT4(x1, y1, r, x4, y4, x3, y3, x2, y2, v, i + 4) \
	TESTCT4(x1, y1, -r, x2, y2, x3, y3, x4, y4, v, i + 8) \
	TESTCT4(x1, y1, -r, x4, y4, x3, y3, x2, y2, v, i + 12)
	TESTCT4B(3, 3, .05f, 3, 1, 4, 4, 2, 5, true, 0);  // circle inside tri
	TESTCT4B(3, 3, 3, 3, 1, 4, 4, 2, 5, true, 16);  // tri inside circle
	TESTCT4B(1, 1.5f, .25f, 3, 1, 4, 4, 2, 5, false, 16);
	TESTCT4B(4, 2, .25f, 3, 1, 4, 4, 2, 5, false, 32);
	TESTCT4B(3, 4.9f, .25f, 3, 1, 4, 4, 2, 5, false, 48);
	TESTCT4B(2, 2, 1, 3, 1, 4, 4, 2, 5, true, 64);  // side
	TESTCT4B(4, 2, 1, 3, 1, 4, 4, 2, 5, true, 80);  // side
	TESTCT4B(3, 4.9f, 1, 3, 1, 4, 4, 2, 5, true, 96);  // side
	TESTCT4B(3, .75f, .5f, 3, 1, 4, 4, 2, 5, true, 112);  // corner
	TESTCT4B(5, 5, 1.5f, 3, 1, 4, 4, 2, 5, true, 128);  // corner
	TESTCT4B(1.5f, 5.5f, 1, 3, 1, 4, 4, 2, 5, true, 144);  // corner

#define TESTCO(x1, y1, r, cx, cy, hx, hy, ox, oy, v, i) \
	if (Cd2CO(Circle2(x1, y1, r), Obb2(cx, cy, ox, oy, hx, hy)) != v) \
		cout << "Failed Cd2CO in test " << i << ".\r\n"; \
	if (Cd2OC(Obb2(cx, cy, ox, oy, hx, hy), Circle2(x1, y1, r)) != v) \
		cout << "Failed Cd2CO reversal (Cd2OC) in test " << i << ".\r\n"
#define TESTCO4(x1, y1, r, cx, cy, hx, hy, ox, oy, v, i) \
	TESTCO(x1, y1, r, cx, cy, hx, hy, ox, oy, v, i); TESTCO(-(x1), y1, r, -(cx), cy, hx, hy, -(ox), oy, v, i + 1); \
	TESTCO(x1, -(y1), r, cx, -(cy), hx, hy, ox, -(oy), v, i + 2); TESTCO(-(x1), -(y1), r, -(cx), -(cy), hx, hy, -(ox), -(oy), v, i + 3);
#define TESTCO44(x1, y1, r, cx, cy, hx, hy, ox, oy, v, i) \
	TESTCO4(x1, y1, r, cx, cy, hx, hy, ox, oy, v, i); TESTCO4(x1, y1, r, cx, cy, -hx, hy, ox, oy, v, i + 4); \
	TESTCO4(x1, y1, r, cx, cy, hx, -hy, ox, oy, v, i + 8); TESTCO4(x1, y1, r, cx, cy, -hx, -hy, ox, oy, v, i + 12)
#define TESTCO44B(x1, y1, r, cx, cy, hx, hy, ox, oy, v, i) \
	TESTCO44(x1, y1, r, cx, cy, hx, hy, ox, oy, v, i) \
	TESTCO44(x1, y1, -r, cx, cy, hx, hy, ox, oy, v, i + 16)
	TESTCO44B(5, 5, .5f, 4, 4, 3, 4,.8321f, .5547f, true, 0);  // circle inside obb
	TESTCO44B(5, 5, 6, 4, 4, 3, 4,.8321f, .5547f, true, 32);  // obb inside circle
	TESTCO44B(8, 7, .5f, 4, 4, 3, 4, .8321f, .5547f, false, 64);
	TESTCO44B(7, .5f, .5f, 4, 4, 3, 4, .8321f, .5547f, false, 96);
	TESTCO44B(.35f, 5, .25f, 4, 4, 3, 4, .8321f, .5547f, false, 128);
	TESTCO44B(2, 8, .5f, 4, 4, 3, 4, .8321f, .5547f, false, 140);
	TESTCO44B(8, 7, 1.5f, 4, 4, 3, 4, .8321f, .5547f, true, 156);
	TESTCO44B(7, .5f, 1.5f, 4, 4, 3, 4, .8321f, .5547f, true, 172);
	TESTCO44B(.35f, 5, 1.25f, 4, 4, 3, 4, .8321f, .5547f, true, 188);
	TESTCO44B(2, 8, 1.5f, 4, 4, 3, 4, .8321f, .5547f, true, 204);

#define TESTLL(x1, y1, x2, y2, x3, y3, x4, y4, v, i) \
	if (Cd2LsLs(LineSeg2(x1, y1, x2, y2), LineSeg2(x3, y3, x4, y4)) != v) \
		cout << "Failed Cd2LsLs in test " << i << ".\r\n"; \
	if (Cd2LsLs(LineSeg2(x3, y3, x4, y4), LineSeg2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2LsLs reversal (Cd2LsLs) in test " << i << ".\r\n"
#define TESTLL4(x1, y1, x2, y2, x3, y3, x4, y4, v, i) \
	TESTLL(x1, y1, x2, y2, x3, y3, x4, y4, v, i); TESTLL(-x1, y1, -x2, y2, -x3, y3, -x4, y4, v, i + 1); \
	TESTLL(x1, -y1, x2, -y2, x3, -y3, x4, -y4, v, i + 2); TESTLL(-x1, -y1, -x2, -y2, -x3, -y3, -x4, -y4, v, i + 3);
	TESTLL4(2, 2, 4, 5, 2, 4, 1, 7, false, 0);
	TESTLL4(2, 2, 4, 5, 4, 2, 6, 1, false, 4);
	TESTLL4(5, 2, 7, 5, 4, 2, 6, 1, false, 8);
	TESTLL4(2, 2, 4, 5, 4, 4, 3, 7, true, 12);

#define TESTLT(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i) \
	if (Cd2LsT(LineSeg2(x1, y1, x2, y2), Triangle2(x3, y3, x4, y4, x5, y5)) != v) \
		cout << "Failed Cd2LsT in test " << i << ".\r\n"; \
	if (Cd2TLs(Triangle2(x3, y3, x4, y4, x5, y5), LineSeg2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2LsT reversal (Cd2TLs) in test " << i << ".\r\n"
#define TESTLT4(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i) \
	TESTLT(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i); TESTLT(-(x2), y1, -(x1), y2, -(x3), y3, -(x4), y4, -(x5), y5, v, i + 1); \
	TESTLT(x1, -(y2), x2, -(y1), x3, -(y3), x4, -(y4), x5, -(y5), v, i + 2); TESTLT(-(x2), -(y2), -(x1), -(y1), -(x3), -(y3), -(x4), -(y4), -(x5), -(y5), v, i + 3);
	// Test with tri verts going ccw and cw
#define TESTLT4B(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i) \
	TESTLT4(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, v, i) \
	TESTLT4(x1, y1, x2, y2, x5, y5, x4, y4, x3, y3, v, i + 4)
	// SAT on triangle axes
	TESTLT4B(6, 0, 5, 2, 2, 2, 7, 4, 4, 7, false, 0);
	TESTLT4B(7, 8, 6, 6, 2, 2, 7, 4, 4, 7, false, 8);
	TESTLT4B(1, 7, 2, 5, 2, 2, 7, 4, 4, 7, false, 16);
	// intersections
	TESTLT4B(2, 5, 5, 2, 2, 2, 7, 4, 4, 7, true, 24);  // tri crosses line
	TESTLT4B(5, 2, 6, 6, 2, 2, 7, 4, 4, 7, true, 32);  // tri crosses line
	TESTLT4B(6, 6, 2, 5, 2, 2, 7, 4, 4, 7, true, 40);  // tri crosses line
	TESTLT4B(2, 5, 4, 4, 2, 2, 7, 4, 4, 7, true, 48);  // line crosses tri edge
	TESTLT4B(5, 2, 4, 4, 2, 2, 7, 4, 4, 7, true, 56);  // line crosses tri edge
	TESTLT4B(6, 6, 4, 4, 2, 2, 7, 4, 4, 7, true, 64);  // line crosses tri edge
	TESTLT4B(3, 3, 4, 4, 2, 2, 7, 4, 4, 7, true, 72);  // line inside tri
	// SAT on Line seg
	TESTLT4B(2, 3, 1.5f, 1, 2, 2, 7, 4, 4, 7, false, 80);

#define TESTLO(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i) \
	if (Cd2LsO(LineSeg2(x1, y1, x2, y2), Obb2(cx, cy, ox, oy, hx, hy)) != v) \
		cout << "Failed Cd2LsO in test " << i << ".\r\n"; \
	if (Cd2OLs(Obb2(cx, cy, ox, oy, hx, hy), LineSeg2(x1, y1, x2, y2)) != v) \
		cout << "Failed Cd2LsO reversal (Cd2OLs) in test " << i << ".\r\n"
#define TESTLO4(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i) \
	TESTLO(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i); TESTLO(-(x1), y1, -(x2), y2, -(cx), cy, hx, hy, -(ox), oy, v, i + 1); \
	TESTLO(x1, -(y1), x2, -(y2), cx, -(cy), hx, hy, ox, -(oy), v, i + 2); TESTLO(-(x1), -(y1), -(x2), -(y2), -(cx), -(cy), hx, hy, -(ox), -(oy), v, i + 3);
#define TESTLO44(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i) \
	TESTLO4(x1, y1, x2, y2, cx, cy, hx, hy, ox, oy, v, i); TESTLO4(x1, y1, x2, y2, cx, cy, -hx, hy, ox, oy, v, i + 4); \
	TESTLO4(x1, y1, x2, y2, cx, cy, hx, -hy, ox, oy, v, i + 8); TESTLO4(x1, y1, x2, y2, cx, cy, -hx, -hy, ox, oy, v, i + 12)
	TESTLO44(5, 5, 6, 6, 4, 4, 3, 4, .8321f, .5547f, true, 0);  // lineseg inside obb
	TESTLO44(.5f, .5f, 7.5f, 7.5f, 4, 4, 3, 4,.8321f, .5547f, true, 16);
	TESTLO44(8, 7, 9, 8, 4, 4, 3, 4, .8321f, .5547f, false, 32);
	TESTLO44(7, .5f, 8, 0, 4, 4, 3, 4, .8321f, .5547f, false, 48);
	TESTLO44(.35f, 4, 0, 3, 4, 4, 3, 4, .8321f, .5547f, false, 64);
	TESTLO44(2, 8, 3, 9.5f, 4, 4, 3, 4, .8321f, .5547f, false, 80);
	TESTLO44(8, 7, 4, 4, 4, 4, 3, 4, .8321f, .5547f, true, 96);
	TESTLO44(7, .5f, 4, 4, 4, 4, 3, 4, .8321f, .5547f, true, 112);
	TESTLO44(.35f, 4, 4, 4, 4, 4, 3, 4, .8321f, .5547f, true, 128);
	TESTLO44(2, 8, 4, 4, 4, 4, 3, 4, .8321f, .5547f, true, 144);

#define TESTTT(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, v, i) \
	if (Cd2TT(Triangle2(x1, y1, x2, y2, x3, y3), Triangle2(x4, y4, x5, y5, x6, y6)) != v) \
		cout << "Failed Cd2TT in test " << i << ".\r\n"; \
	if (Cd2TT(Triangle2(x4, y4, x5, y5, x6, y6), Triangle2(x1, y1, x2, y2, x3, y3)) != v) \
		cout << "Failed Cd2TT reversal (Cd2TT) in test " << i << ".\r\n"
	// Negatives of position and orientation
#define TESTTT4(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, v, i) \
	TESTTT(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, v, i); TESTTT(-(x1), y1, -(x2), y2, -(x3), y3, -(x4), y4, -(x5), y5, -(x6), y6, v, i + 1); \
	TESTTT(x1, -(y1), x2, -(y2), x3, -(y3), x4, -(y4), x5, -(y5), x6, -(y6), v, i + 2); TESTTT(-(x1), -(y1), -(x2), -(y2), -(x3), -(y3), -(x4), -(y4), -(x5), -(y5), -(x6), -(y6), v, i + 3);
	// cw and ccw order of tris
#define TESTTT44(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, v, i) \
	TESTTT4(x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, v, i); TESTTT4(x3, y3, x2, y2, x1, y1, x4, y4, x5, y5, x6, y6, v, i + 4); \
	TESTTT4(x1, y1, x2, y2, x3, y3, x6, y6, x5, y5, x4, y4, v, i + 8); TESTTT4(x3, y3, x2, y2, x1, y1, x6, y6, x5, y5, x4, y4, v, i + 12);
	TESTTT44(2, 2, 5, 3, 3, 6, 4, 2, 5, 6, 2, 5, true, 0);  // Both intersect, no points inside though
	TESTTT44(4, 4, 4.5f, 5, 3, 4.5f, 4, 2, 5, 6, 2, 5, true, 16);  // a inside b
	TESTTT44(2, 2, 5, 3, 3, 6, 3, 4, 4, 4.5f, 3.5f, 5, true, 32);  // b inside a
	// Reverses of these covered in macro
	TESTTT44(2, 2, 5, 3, 3, 6, 2, 5, 5, 6, 3, 9, true, 48);  // b top point through a bottom edge
	TESTTT44(2, 2, 5, 3, 3, 6, 4, 1, 7, 2, 5, 5, true, 64);  // b left point through a right edge
	TESTTT44(2, 2, 5, 3, 3, 6, 0, 0, 3, 1, 1, 5, true, 80);  // b right point through a left edge
	TESTTT44(3, -1, 6, 0, 4, 3, 4, 2, 5, 6, 2, 5, true, 96);  // dual point intersection
	// SAT
	TESTTT44(2, 2, 5, 3, 3, 6, 6, 2, 7, 6, 4, 5, false, 112);
	TESTTT44(2, 2, 5, 3, 3, 6, 1, 2, 2, 6, -1, 5, false, 128);
	TESTTT44(2, 2, 5, 3, 3, 6, 4, 6, 5, 10, 2, 9, false, 144);

#define TESTTO(x1, y1, x2, y2, x3, y3, cx, cy, hx, hy, ox, oy, v, i) \
	if (Cd2TO(Triangle2(x1, y1, x2, y2, x3, y3), Obb2(cx, cy, ox, oy, hx, hy)) != v) \
		cout << "Failed Cd2TO in test " << i << ".\r\n"; \
	if (Cd2OT(Obb2(cx, cy, ox, oy, hx, hy), Triangle2(x1, y1, x2, y2, x3, y3)) != v) \
		cout << "Failed Cd2TO reversal (Cd2OT) in test " << i << ".\r\n"
	// Negatives of position and orientation
#define TESTTO4(x1, y1, x2, y2, x3,y3, cx, cy, hx, hy, ox, oy, v, i) \
	TESTTO(x1, y1, x2, y2, x3, y3, cx, cy, hx, hy, ox, oy, v, i); TESTTO(-(x1), y1, -(x2), y2, -(x3), y3, -(cx), cy, hx, hy, -(ox), oy, v, i + 1); \
	TESTTO(x1, -(y1), x2, -(y2), x3, -(y3), cx, -(cy), hx, hy, ox, -(oy), v, i + 2); TESTTO(-(x1), -(y1), -(x2), -(y2), -(x3), -(y3), -(cx), -(cy), hx, hy, -(ox), -(oy), v, i + 3);
	// Negatives of halfW/halfH
#define TESTTO44(x1, y1, x2, y2, x3, y3, cx, cy, hx, hy, ox, oy, v, i) \
	TESTTO4(x1, y1, x2, y2, x3, y3, cx, cy, hx, hy, ox, oy, v, i); TESTTO4(x1, y1, x2, y2, x3, y3, cx, cy, -hx, hy, ox, oy, v, i + 4); \
	TESTTO4(x1, y1, x2, y2, x3, y3, cx, cy, hx, -hy, ox, oy, v, i + 8); TESTTO4(x1, y1, x2, y2, x3, y3, cx, cy, -hx, -hy, ox, oy, v, i + 12)
	// cw and ccw order of tris
#define TESTTO44B(x1, y1, x2, y2, x3, y3, cx, cy, hx, hy, ox, oy, v, i) \
	TESTTO44(x1, y1, x2, y2, x3, y3, cx, cy, hx, hy, ox, oy, v, i) \
	TESTTO44(x3, y3, x2, y2, x1, y1, cx, cy, hx, hy, ox, oy, v, i + 16)
	// SAT on triangle axes
	TESTTO44B(2, 2, 5, 3, 3, 6, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, false, 0);
	TESTTO44B(10, 3, 13, 4, 11, 7, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, false, 32);
	TESTTO44B(6, 9, 9, 10, 7, 13, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, false, 64);
	// intersections
	TESTTO44B(4, 4, 7, 5, 5, 8, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, true, 96);
	TESTTO44B(9, 3, 12, 4, 10, 7, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, true, 128);
	TESTTO44B(6, 8, 9, 9, 7, 12, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, true, 160);
	TESTTO44B(4, 3, 5, 4, 3, 5, 4, 4, 3, 4,.8321f, .5547f, true, 192);  // triangle inside obb
	TESTTO44B(4, 1, 8, 7, 1, 6, 4, 4, .5f, 1, .8321f, .5547f, true, 224);  // obb inside triangle
	// SAT on OBB axes
	TESTTO44B(2, 2, 5, 3, 3, 6, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, false, 256);
	TESTTO44B(9, -1, 12, 0, 10, 3, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, false, 288);
	TESTTO44B(10, 6, 13, 7, 11, 10, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, false, 320);
	TESTTO44B(2, 9, 5, 10, 3, 13, 7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, false, 352);
	
#define TESTOO(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i) \
	if (Cd2OO(Obb2(acx, acy, aox, aoy, ahx, ahy), Obb2(bcx, bcy, box, boy, bhx, bhy)) != v) \
		cout << "Failed Cd2OO in test " << i << ".\r\n"; \
	if (Cd2OO(Obb2(bcx, bcy, box, boy, bhx, bhy), Obb2(acx, acy, aox, aoy, ahx, ahy)) != v) \
		cout << "Failed Cd2OO reversal (Cd2OO) in test " << i << ".\r\n"
	// Negatives of position and orientation
#define TESTOO4(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i) \
	TESTOO(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i); TESTOO(-(acx), acy, ahx, ahy, -(aox), aoy, -(bcx), bcy, bhx, bhy, -(box), boy, v, i + 1); \
	TESTOO(acx, -(acy), ahx, ahy, aox, -(aoy), bcx, -(bcy), bhx, bhy, box, -(boy), v, i + 2); TESTOO(-(acx), -(acy), ahx, ahy, -(aox), -(aoy), -(bcx), -(bcy), bhx, bhy, -(box), -(boy), v, i + 3);
	// Negatives of halfW/halfH
#define TESTOO44(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i) \
	TESTOO4(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i); TESTOO4(acx, acy, -ahx, ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i + 4); \
	TESTOO4(acx, acy, ahx, -ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i + 8); TESTOO4(acx, acy, -ahx, -ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i + 12)
#define TESTOO44B(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i) \
	TESTOO44(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, bhx, bhy, box, boy, v, i); TESTOO44(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, -bhx, bhy, box, boy, v, i + 16); \
	TESTOO44(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, bhx, -bhy, box, boy, v, i + 32); TESTOO44(acx, acy, ahx, ahy, aox, aoy, bcx, bcy, -bhx, -bhy, box, boy, v, i + 48)
	// Note that we reverse roles in macro
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 10, 2, .77f, 1.4f, -.8945f, .4472f, false, 0);  // b top of a
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 11, 9, .77f, 1.4f, -.8945f, .4472f, false, 64);  // b right of a
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 6, 11, .77f, 1.4f, -.8945f, .4472f, false, 128);  // b bottom of a
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 5, 4, .77f, 1.4f, -.8945f, .4472f, false, 192);  // b left of a
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 9, 4, .77f, 1.4f, -.8945f, .4472f, true, 256);  // b top of a
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 9.5f, 8, .77f, 1.4f, -.8945f, .4472f, true, 320);  // b right of a
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 6, 9.5f, .77f, 1.4f, -.8945f, .4472f, true, 384);  // b bottom of a
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 6, 5, .77f, 1.4f, -.8945f, .4472f, true, 448);  // b left of a
	TESTOO44B(7.5f, 6.5f, 1.8f, 2, .8321f, .5547f, 7.5f, 6.5f, .77f, 1.4f, -.8945f, .4472f, true, 448);  // b inside a

	//NN
	// If triangle-to-triangle works, we can be confident that NN works... it uses NN directly.

	return 0;
}
