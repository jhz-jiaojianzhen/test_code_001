#ifndef __GEOMETRY__H
#define __GEOMETRY__H

#ifdef __cplusplus
extern "C"
{
#endif         // __cplusplus


/* 点 */
typedef struct
{
	float x;
	float y;
}Point;

typedef struct
{
	int x;
	int y;
}Grid_Point;


/* 线段AB */
typedef struct segment
{
	Point A;
	Point B;
}Segment;

/* 直线
 *	直线方程一般式：Ax+By+C=0
 */
typedef struct line
{
	float A;
	float B;
	float C;
}Line;

/* 三角形ABC */
typedef struct triangle
{
	Point A;
	Point B;
	Point C;
}Triangle;

/* 边平行于坐标轴的矩形 */
typedef struct rectangle
{
	float xmin;
	float ymin;
	float xmax;
	float ymax;
}Rect_angle;

/* MBR:最小包容矩形 */
typedef Rect_angle MBR;

/* 圆 */
typedef struct circle
{
	Point centre;
	float radius;
}Circle;


//两点的距离
float DistanceOfPoints(Point A, Point B);

 //点到直线的距离
float DistanceFromPointToLine(Point P, Line L);

//计算点到直线的垂点
Point VerticalPoint(Point p, Line l);

/* 返回两直线的交点 */
Point LinesIntersection(Line m, Line n, int *flag);


//两个不同点A,B确定一条直线，AB相同返回的值全0
Line LineMake(Point A, Point B);

float DistanceFromPointToSegment(Point A, Segment s);

#ifdef __cplusplus
}
#endif         // __cplusplus
#endif
