#pragma once
#include"Graphic.h"
#include"GeoDefine3D.h"
#include "GeoDefine.h"
struct Raster
{	
	//光栅化线
	void DDA_Line(int x1, int y1, int x2, int y2, Color penc);//微分法光栅化绘制线
	void bresenham(int x0, int y0, int x1, int y1, Color penc);
	void clerarCanvas(Layer* g_Layer);
	//任意斜率

	void drawPolygonline(PixelPoint* pts, int count, Color color);//光栅化多边形
	void drawCircle(double x, double y, double r, unsigned color);
	//X扫描线填充多边形
	void polygonScan(PixelPoint* pts, int count, Color penc);//多边形X扫描线活性边表法填充
	void polygonScan(Vertex3D* pts, int count, Color penc);//z_buffer,对深度z填充
	void drawPolygon(PixelPoint* data, int size, unsigned color);
	void clerarCanvas();
	
	//多边形X扫描线活性边表法填充
	static void MidPointClipAlgorithm(PixelPoint pt1, PixelPoint pt2, Layer* container);
	static void Sutherland_Hodgeman(PixelPoint rectPt1, PixelPoint rectPt2, Layer* container);
	void CohenSutherland(PixelPoint pt1, PixelPoint pt2, Layer* container);
	//区域填充
	void BoundaryFill(int x, int y, unsigned bdrColor, unsigned newColor);
	
	void PointFill(int x, int y, unsigned newColor);
	double ToGetDeepthZ(Vec4 pt0, Vec4 pt1, Vec4 pt2, int x, int y);
};
extern Raster g_Raster;