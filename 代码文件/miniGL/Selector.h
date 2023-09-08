#pragma once
#include <vector>
#include "GeoDefine.h"

class Renderer2D;
class Selector
{
public:	
	Selector(Renderer2D* pRenderer2D);

	int selectLayerByPoint(Layer* pLayer , Point2D& pt, bool isMultipleSelect );
	int selectGeometryByPoint(Geometry* pGeometry, Point2D& pt, bool isMultipleSelect );
	void selectGeometry(Geometry* pGeometry, bool isMultipleSelect );
	void clear();
	bool empty();
	std::vector<Geometry*>& getSelectionSet();

	int ptInGeometry(Geometry* pGeometry, Point2D& pt );
	int ptOnLine(const Point2D& pt, const Point2D& p1, const Point2D& p2 );
	int ptInPolygon(const Point2D& pt, const std::vector<Point2D>& pts );

	void setPixelTolerance(double d);

	std::vector<Geometry*> mSelectionSet;

private:
	double mPixelTolerance;
	Renderer2D* mpRenderer2D;
};

