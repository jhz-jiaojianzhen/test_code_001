#include <math.h>
#include "geometry.h"


//两点之间的距离，线段的长度
float DistanceOfPoints(Point A, Point B)
{
	return (float)sqrt((A.x-B.x)*(A.x-B.x) + (A.y-B.y)*(A.y-B.y));
}

//点到直线的距离
float DistanceFromPointToLine(Point P, Line L)
{
	return fabs(L.A*P.x + L.B*P.y + L.C)/sqrt(L.A*L.A+L.B*L.B);
}

//计算点到直线的垂点
Point VerticalPoint(Point p, Line l)
{
	Point v;
	float tmp = l.A * l.A + l.B * l.B;
	v.x = (l.B * l.B * p.x - l.A * l.B * p.y - l.A * l.C) / tmp;
	v.y = (l.A * l.A * p.y - l.A * l.B * p.x - l.B * l.C) / tmp;

	return v;
}

/* 返回两直线的交点 */
Point LinesIntersection(Line m, Line n, int *flag)
{
	double d = n.A * m.B - m.A * n.B;
	if (d == 0)
	{
		*flag = 0;
		Point er = {0,0};
		return er;
	}
	Point i;
	i.x = (n.B * m.C - m.B * n.C) / d;
	i.y = (m.A * n.C - n.A * m.C) / d;
	*flag = 1;
	return i;
}

/* 两个不同点A,B确定一条直线，AB相同返回的值全0
 *  直线方程：Ax+By+c=0
 *  A = y2 - y1;
 *  B = x1 - x2;
 *  C = -A*x1 - B*y1 = x2*y1 - x1*y2;
 */
Line LineMake(Point A, Point B)
{
	Line l;
	l.A = B.y - A.y;
	l.B = A.x - B.x;
	l.C = B.x * A.y - A.x * B.y;
	return l;
}

float DistanceFromPointToSegment(Point p, Segment s)//计算点到线段的最短距离（方向向量法）
{
	float cross = (s.B.x - s.A.x) * (p.x - s.A.x) + (s.B.y - s.A.y) * (p.y - s.A.y);
	if(cross<=0)
		return sqrt((p.x-s.A.x)*(p.x-s.A.x) + (p.y-s.A.y)*(p.y-s.A.y));
	float d2 = (s.B.x - s.A.x) * (s.B.x - s.A.x) + (s.B.y - s.A.y) * (s.B.y - s.A.y);
	if(cross>=d2)
		return sqrt((p.x-s.B.x)*(p.x-s.B.x) + (p.y-s.B.y)*(p.y-s.B.y));

	float r = cross/d2;
	float px = s.A.x + (s.B.x - s.A.x) * r;
	float py = s.A.y + (s.B.y - s.A.y) * r;
	return sqrt(p.x - px) * (p.x - px) + (py - s.A.y) * (py - s.A.y);
}
