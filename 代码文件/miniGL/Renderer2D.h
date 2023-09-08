#pragma once

#include "Matrix3.h"
#include "Graphic.h"
#include <vector>

struct Geometry;
struct Dataset;
struct Layer;
class Selector;
class Renderer2D
{
public:

	Renderer2D() { mSelectColor = RED; };

	//图层初始化
	void init(double dx, double dy, double w, double h, int WWidth, int WHeight);

	//复原视图
	virtual void reset();

	//设置窗口，2D是正交投影
	virtual void setWindow(double left, double right, double bottom, double top);

	//视口大小改变
	virtual void setViewport(int x, int y, int width, int height);

	//绘制数据集包含的图层
	virtual void Renderer2D::render(Dataset* pDataset);

	//绘制图层
	virtual void Renderer2D::render(Layer* pLayer);

	//绘制图形对象
	virtual void Renderer2D::render(Geometry* pGeometryDef);

	void applyViewMatrix(Matrix3& m);

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


	//网格背景
	void renderBackground();

	Matrix3 mTransformMatrix;//变换矩阵，记录所有视口、模型变换矩阵	
protected:	

	Color mSelectColor;//选择集渲染颜色
	Matrix3 m;//变换矩阵	
	double mOrigXCenter_map, mOrigYCenter_map, mOrigWidth_map, mOrigHeight_map;//对应窗口原点的地图点
	double mOrigScale;
	double mX_win, mY_win;
	double mWidth_win, mHeight_win;
};

