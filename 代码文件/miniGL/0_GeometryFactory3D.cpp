#pragma once
#include "0_GeometryFactory3D.h"
#include "Graphic.h"

Geometry3D* GeometryFactory3D::createPointGeometry3D(double x, double y,double z)
{
	return new PointGeometry3D(x, y ,z);
}

Geometry3D* GeometryFactory3D::createPolylineGeometry3D(PixelPoint3D* pts, int size)
{
	PolylineGeometry3D* pGeometry = new PolylineGeometry3D();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y, pts[i].z);
	}
	return pGeometry;
}

Geometry3D* GeometryFactory3D::createPolylineGeometry3D(Point3D* pts, int size)
{
	PolylineGeometry3D* pGeometry = new PolylineGeometry3D();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y, pts[i].z);
	}
	return pGeometry;
}

Geometry3D* GeometryFactory3D::createPolylineGeometry3D(PixelPoint3D& pt1, PixelPoint3D& pt2)
{
	PolylineGeometry3D* pGeometry = new PolylineGeometry3D();
	pGeometry->addPoint(pt1.x, pt1.y, pt1.z);
	pGeometry->addPoint(pt2.x, pt2.y, pt1.z);
	return pGeometry;
}

Geometry3D* GeometryFactory3D::createPolygonGeometry3D(PixelPoint3D* pts, int size)
{
	PolygonGeometry3D* pGeometry = new PolygonGeometry3D();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y, pts[i].z);
	}
	return pGeometry;
}

Geometry3D* GeometryFactory3D::createPolygonOutlineGeometry3D(PixelPoint3D* pts, int size)
{
	PolylineGeometry3D* pGeometry = new PolylineGeometry3D();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y,pts[i].z);
	}
	pGeometry->addPoint(pts[0].x, pts[0].y, pts[0].z);
	return pGeometry;
}

Geometry3D* GeometryFactory3D::createPolygonGeometry3D(Point3D* pts, int size)
{
	PolygonGeometry3D* pGeometry = new PolygonGeometry3D();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y, pts[i].z);
	}
	return pGeometry;
}

Geometry3D* GeometryFactory3D::createPolygonOutlineGeometry3D(Point3D* pts, int size)
{
	PolylineGeometry3D* pGeometry = new PolylineGeometry3D();
	for (int i = 0; i < size; ++i)
	{
		pGeometry->addPoint(pts[i].x, pts[i].y, pts[i].z);
	}
	pGeometry->addPoint(pts[0].x, pts[0].y, pts[0].z);
	return pGeometry;
}

Geometry3D* GeometryFactory3D::createCircleGeometry3D(double x, double y, double z ,double r)
{
	return new CircleGeometry3D(x, y,z, r);
}

Geometry3D* GeometryFactory3D::createCircleGeometry3D(double x1, double y1,double z1, double x2, double y2,double z2)
{
	double dx = x1 - x2;
	double dy = y1 - y2;
	double dz = z1 - z2;
	return new CircleGeometry3D((x1+x2)*0.5, (y1+y2)*0.5, (z1+z2)*0.5 , (sqrt(dx * dx + dy * dy+ dz*dz))*0.5);
}

Geometry3D* GeometryFactory3D::createCircleGeometry3D(PixelPoint3D* pts, int size)
{
	if (size < 2) return NULL;
	return createCircleGeometry3D(pts[0].x, pts[0].y, pts[0].z, pts[1].x, pts[1].y, pts[1].z);
}

//Geometry3D* GeometryFactory3D::createEllipseGeometry3D(double x1, double y1,double z1, double x2, double y2,double z2)
//{
//	if (x1 > x2)swap(x1, x2);
//	if (y1 > y2)swap(y1, y2);
//	if (z1 > z2)swap(z1, z2);
//
//	return new EllipseGeometry3D(x1, y1,z1, x2, y2,z2);
//}

//Geometry3D* GeometryFactory3D::createEllipseGeometry3D(PixelPoint3D* pts, int size)
//{
//	if (size < 2) return NULL;
//	return createEllipseGeometry3D(pts[0].x, pts[0].y, pts[0].z, pts[1].x, pts[1].y, pts[1].z);
//}

Geometry3D* GeometryFactory3D::creatRectangleGeometry3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
	if (x1 > x2)swap(x1, x2);
	if (y1 > y2)swap(y1, y2);

	PolygonGeometry3D* pGeometry = new PolygonGeometry3D();
	pGeometry->addPoint(x1, y1, z1);
	pGeometry->addPoint(x1, y2, z1);
	pGeometry->addPoint(x1, y2, z2);
	pGeometry->addPoint(x1, y1, z2);
	pGeometry->addPoint(x2, y1, z2);
	pGeometry->addPoint(x2, y2, z2);
	pGeometry->addPoint(x2, y2, z1);
	pGeometry->addPoint(x2, y1, z1);
	
	return pGeometry;
}

Geometry3D* GeometryFactory3D::creatRectangleGeometry3D(PixelPoint3D* pts, int size)
{
	if (size < 2) return NULL;
	return creatRectangleGeometry3D(pts[0].x, pts[0].y, pts[0].z, pts[1].x, pts[1].y, pts[1].z);
}

Geometry3D* GeometryFactory3D::creatRectangleGeometry3D(Point3D* pts, int size)
{
	if (size < 2) return NULL;
	return creatRectangleGeometry3D(pts[0].x, pts[0].y, pts[0].z, pts[1].x, pts[1].y, pts[1].z);
}

Geometry3D* GeometryFactory3D::creatRectangleOutlineGeometry3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
	if (x1 > x2)swap(x1, x2);
	if (y1 > y2)swap(y1, y2);

	PolylineGeometry3D* pGeometry = new PolylineGeometry3D();
	pGeometry->addPoint(x1, y1, z1);
	pGeometry->addPoint(x1, y2, z1);
	pGeometry->addPoint(x1, y2, z2);
	pGeometry->addPoint(x1, y1, z2);
	pGeometry->addPoint(x2, y1, z2);
	pGeometry->addPoint(x2, y2, z2);
	pGeometry->addPoint(x2, y2, z1);
	pGeometry->addPoint(x2, y1, z1);
	pGeometry->addPoint(x1, y1, z1);

	return pGeometry;
}

Geometry3D* GeometryFactory3D::creatRectangleOutlineGeometry3D(PixelPoint3D* pts, int size)
{
	if (size < 2) return NULL;
	return creatRectangleOutlineGeometry3D(pts[0].x, pts[0].y, pts[0].z, pts[1].x, pts[1].y, pts[1].z);
}

Geometry3D* GeometryFactory3D::creatRectangleOutlineGeometry3D(Point3D* pts, int size)
{
	if (size < 2) return NULL;
	return creatRectangleOutlineGeometry3D(pts[0].x, pts[0].y, pts[0].z, pts[1].x, pts[1].y, pts[1].z);
}
