#pragma once
#include "Matrix3.h"

struct Geometry;
struct Dataset;
struct Layer;
class Editor
{
public:
	//绘制数据集包含的图层
	void apply(Dataset* pDataset, Matrix3& m);

	//绘制图层
	void apply(Layer* pLayer, Matrix3& m);

	//绘制图形对象
	void apply(Geometry* pGeometryDef, Matrix3& m);

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
};
