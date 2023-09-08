#pragma once
#include "0_Matrix4.h"
//#include "Matrix3.h"

//#事实上这里我没有想好怎么进行从3D到2D的展示:像素点是x，y。

struct Geometry3D;
struct Dataset3D;
struct Layer3D;
class Editor3D
{
public:
	//绘制数据集包含的图层
	void apply(Dataset3D* pDataset, Matrix4& m);

	//绘制图层
	void apply(Layer3D* pLayer, Matrix4& m);

	//绘制图形对象
	void apply(Geometry3D* pGeometryDef, Matrix4& m);

	//平移，以像素为单位
	void translate(double dx, double dy, double dz);

	//旋转，逆时针为正， 以弧度为单位
	void rotate(double rad);

	//定点旋转，逆时针为正， 以弧度为单位
	void rotate(double x, double y, double z,double rad);

	//中心放缩
	void scale(double s);

	//定点放缩
	void scale(double x, double y, double z,double s);
};
