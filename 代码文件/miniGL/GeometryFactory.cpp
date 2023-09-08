#include "GeometryFactory.h"
#include "Graphic.h"
//创建几何点
Geometry* GeometryFactory::createPointGeometry(double x, double y)
{
	return new PointGeometry(x, y);
}
//线
Geometry* GeometryFactory::createPolylineGeometry(PixelPoint* pts, int size)
{
	PolylineGeometry* pGeometry = new PolylineGeometry();
	for (int i = 0; i < size ; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y);
	}
	return pGeometry;
}

Geometry* GeometryFactory::createPolylineGeometry(Point2D* pts, int size)
{
	PolylineGeometry* pGeometry = new PolylineGeometry();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y);
	}
	return pGeometry;
}

Geometry* GeometryFactory::createPolylineGeometry(PixelPoint& pt1, PixelPoint& pt2)
{
	PolylineGeometry* pGeometry = new PolylineGeometry();
	pGeometry->addPoint(pt1.x, pt1.y);
	pGeometry->addPoint(pt2.x, pt2.y);
	return pGeometry;
}

//多边形
Geometry* GeometryFactory::createPolygonGeometry(PixelPoint* pts, int size)
{
	PolygonGeometry* pGeometry = new PolygonGeometry();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y);
	}
	return pGeometry;
}

Geometry* GeometryFactory::createPolygonOutlineGeometry(PixelPoint* pts, int size)
{
	PolylineGeometry* pGeometry = new PolylineGeometry();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y);
	}
	pGeometry->addPoint(pts[0].x, pts[0].y);
	return pGeometry;
}

Geometry* GeometryFactory::createPolygonGeometry(Point2D* pts, int size)
{
	PolygonGeometry* pGeometry = new PolygonGeometry();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y);
	}
	return pGeometry;
}

Geometry* GeometryFactory::createPolygonOutlineGeometry(Point2D* pts, int size)
{
	PolylineGeometry* pGeometry = new PolylineGeometry();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y);
	}
	pGeometry->addPoint(pts[0].x, pts[0].y);
	return pGeometry;
}
//圆形
Geometry* GeometryFactory::createCircleGeometry(double x, double y, double r)
{
	return new CircleGeometry(x, y, r );
}

Geometry* GeometryFactory::createCircleGeometry(double x1, double y1, double x2, double y2)
{
	double dx = x1 - x2;
	double dy = y1 - y2;
	return new CircleGeometry( x1, y1, sqrt(dx * dx + dy * dy));
}

Geometry* GeometryFactory::createCircleGeometry(PixelPoint* pts, int size)
{
	if (size < 2) return NULL;
	return createCircleGeometry(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
}
//椭圆形
Geometry* GeometryFactory::createEllipseGeometry(double x1, double y1, double x2, double y2)
{
	if (x1 > x2)swap(x1, x2);
	if (y1 > y2)swap(y1, y2);

	return new EllipseGeometry(x1, y1, x2, y2 );
}

Geometry* GeometryFactory::createEllipseGeometry(PixelPoint* pts, int size)
{
	if (size < 2) return NULL;
	return createEllipseGeometry(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
}
//矩形
Geometry* GeometryFactory::creatRectangleGeometry(double x1, double y1, double x2, double y2)
{
	if (x1 > x2)swap(x1, x2);
	if (y1 > y2)swap(y1, y2);

	PolygonGeometry* pGeometry = new PolygonGeometry();
	pGeometry->addPoint(x1, y1);
	pGeometry->addPoint(x2, y1);
	pGeometry->addPoint(x2, y2);
	pGeometry->addPoint(x1, y2);
	return pGeometry;
}

Geometry* GeometryFactory::creatRectangleGeometry(PixelPoint* pts, int size)
{
	if (size < 2) return NULL;
	return creatRectangleGeometry(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
}

Geometry* GeometryFactory::creatRectangleGeometry(Point2D* pts, int size)
{
	if (size < 2) return NULL;
	return creatRectangleGeometry(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
}

Geometry* GeometryFactory::creatRectangleOutlineGeometry(double x1, double y1, double x2, double y2)
{
	if (x1 > x2)swap(x1, x2);
	if (y1 > y2)swap(y1, y2);

	PolylineGeometry* pGeometry = new PolylineGeometry();
	pGeometry->addPoint(x1, y1);
	pGeometry->addPoint(x2, y1);
	pGeometry->addPoint(x2, y2);
	pGeometry->addPoint(x1, y2);
	pGeometry->addPoint(x1, y1);

	return pGeometry;
}

Geometry* GeometryFactory::creatRectangleOutlineGeometry(PixelPoint* pts, int size)
{
	if (size < 2) return NULL;
	return creatRectangleOutlineGeometry(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
}

Geometry* GeometryFactory::creatRectangleOutlineGeometry(Point2D* pts, int size)
{
	if (size < 2) return NULL;
	return creatRectangleOutlineGeometry(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
}
