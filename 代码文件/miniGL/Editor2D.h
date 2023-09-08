#pragma once
#include <vector>
#include "Matrix3.h"

class Renderer2D;
struct Geometry;
class Editor2D
{
public:
	Editor2D(Renderer2D* pRenderer2D) {
		mpRenderer2D = pRenderer2D;
	}

	//平移，以像素为单位
	void translate(double dx, double dy);

	//旋转，逆时针为正， 以弧度为单位
	void rotate(double rad);

	//定点旋转，逆时针为正， 以弧度为单位
	void rotate(double x, double y, double rad);

	//中心放缩
	void scale(double s);

	//定点放缩
	void scale(double x, double y, double s);

	void updateGeometrySet(std::vector<Geometry*>& geometrySet );

	void updateGeometry(Geometry* pGeometry) {
		updateGeometry(pGeometry, m);
	}

	static void updateGeometry(Geometry* pGeometry , Matrix3& m );

	Renderer2D* mpRenderer2D;
	Matrix3 m;
};

