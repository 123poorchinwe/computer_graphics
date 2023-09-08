#pragma once

#include "0_GeoDefine3D.h"

struct PixelPoint;
class GeometryFactory3D
{
public:
	static Geometry3D* createPointGeometry3D(double x, double y, double z);

	static Geometry3D* createPolylineGeometry3D(PixelPoint3D* pts, int size);
	static Geometry3D* createPolylineGeometry3D(Point3D* pts, int size);
	static Geometry3D* createPolylineGeometry3D(PixelPoint3D& pt1, PixelPoint3D& pt2);

	static Geometry3D* createPolygonGeometry3D(PixelPoint3D* pts, int size);
	static Geometry3D* createPolygonGeometry3D(Point3D* pts, int size);

	static Geometry3D* createPolygonOutlineGeometry3D(PixelPoint3D* pts, int size);
	static Geometry3D* createPolygonOutlineGeometry3D(Point3D* pts, int size);

	static Geometry3D* createCircleGeometry3D(PixelPoint3D* pts, int size);
	static Geometry3D* createCircleGeometry3D(double x, double y,double z, double r);
	static Geometry3D* createCircleGeometry3D(double x1, double y1, double z1, double x2, double y2, double z2);

	/*static Geometry3D* createEllipseGeometry3D(PixelPoint3D* pts, int size);
	static Geometry3D* createEllipseGeometry3D(double x1, double y1, double z1,double x2, double y2,double z2);*/
	static Geometry3D* creatRectangleGeometry3D(PixelPoint3D* pts, int size);
	static Geometry3D* creatRectangleGeometry3D(Point3D* pts, int size);
	static Geometry3D* creatRectangleGeometry3D(double x1, double y1, double z1, double x2, double y2, double z2);

	static Geometry3D* creatRectangleOutlineGeometry3D(PixelPoint3D* pts, int size);
	static Geometry3D* creatRectangleOutlineGeometry3D(Point3D* pts, int size);
	static Geometry3D* creatRectangleOutlineGeometry3D(double x1, double y1, double z1, double x2, double y2, double z2);
};

