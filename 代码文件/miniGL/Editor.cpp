#define _CRT_SECURE_NO_WARNINGS 1
#include "Editor.h"
#include "GeoDefine.h"
#include "Vec3.h"
#include "Graphic.h"

void Editor::apply(Dataset* pDataset, Matrix3& m)
{
	for (int i = pDataset->getLayerCount() - 1; i >= 0; --i)
	{
		Layer* pLayer = (*pDataset)[i];
		apply(pLayer, m);
	}
}

void Editor::apply(Layer* pLayer, Matrix3& m)
{
	for (int i = 0, size = pLayer->getGeometryCount(); i < size; ++i)
	{
		apply((*pLayer)[i], m);
	}
}
//编辑
void Editor::apply(Geometry* pGeometryDef, Matrix3& m)
{
	switch (pGeometryDef->getGeomType())
	{
	case gtPoint:
	{
		PointGeometry* pGeometry = (PointGeometry*)pGeometryDef;
		Vec3 v(pGeometry->x, pGeometry->y);
		v *= m;
		pGeometry->x = v.x();
		pGeometry->y = v.y();
	}
	break;
	case gtPolyline:
	case gtPolygon:
	{
		PolylineGeometry* pGeometry = (PolylineGeometry*)pGeometryDef;//转化为实际图形类型
		vector<Point2D>& pts = pGeometry->getPts();//获得图形顶点集合
		size_t ptsCount = pts.size();
		for (int i = 0; i < ptsCount; ++i)
		{
			Vec3 v(pts[i].x, pts[i].y);
			v *= m;
			pts[i].x = v.x();
			pts[i].y = v.y();
		}
	}
	break;
	}
}
//平移
void Editor::translate(double dx, double dy)
{
	Matrix3 m;
	m *= Matrix3::translate(dx, dy);
}


//旋转，逆时针为正， 以弧度为单位
void Editor::rotate(double rad)
{
	Matrix3 m;
	m *= Matrix3::rotate(rad);
}

//定点旋转，逆时针为正， 以弧度为单位
void Editor::rotate(double x, double y, double rad)
{
	Matrix3 m;
	m.makeTranslate(-x, -y);
	m *= m.rotate(rad);
	m *= m.makeTranslate(x, y);

}

//中心放缩
void Editor::scale(double s)
{
	Matrix3 m;
	m *= Matrix3::scale(s, s);
}

//定点放缩
void Editor::scale(double x, double y, double s)
{
	Matrix3 m;
	m.makeTranslate(-x, -y);
	m *= m.scale(s, s);
	m *= m.makeTranslate(x, y);
}