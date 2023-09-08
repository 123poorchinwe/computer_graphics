#pragma once

#include "GeoDefine.h"

struct PixelPoint;
//声明了二维图形绘制的方法
class GeometryFactory
{
public :
	//点
	static Geometry* createPointGeometry( double x, double y );

	//线
	static Geometry* createPolylineGeometry( PixelPoint* pts, int size);
	static Geometry* createPolylineGeometry(Point2D* pts, int size);
	static Geometry* createPolylineGeometry(PixelPoint& pt1, PixelPoint& pt2);

	//多边形
	static Geometry* createPolygonGeometry( PixelPoint* pts, int size);	
	static Geometry* createPolygonGeometry(Point2D* pts, int size);

	static Geometry* createPolygonOutlineGeometry(PixelPoint* pts, int size);
	static Geometry* createPolygonOutlineGeometry(Point2D* pts, int size);
	//圆形
	static Geometry* createCircleGeometry(PixelPoint* pts, int size);
	static Geometry* createCircleGeometry(double x, double y , double r );
	static Geometry* createCircleGeometry(double x1, double y1, double x2, double y2);
	//椭圆形
	static Geometry* createEllipseGeometry(PixelPoint* pts, int size);
	static Geometry* createEllipseGeometry(double x1, double y1, double x2, double y2 );
	//矩形
	static Geometry* creatRectangleGeometry(double x1, double y1, double x2, double y2);
	static Geometry* creatRectangleGeometry(PixelPoint* pts, int size);
	static Geometry* creatRectangleGeometry(Point2D* pts, int size);
	static Geometry* creatRectangleOutlineGeometry(double x1, double y1, double x2, double y2);
	static Geometry* creatRectangleOutlineGeometry(PixelPoint* pts, int size);
	static Geometry* creatRectangleOutlineGeometry(Point2D* pts, int size);
	

};

