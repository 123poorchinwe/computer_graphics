#pragma once
#include"Graphic.h"
#include"GeoDefine3D.h"
#include "GeoDefine.h"
struct Raster
{	
	//��դ����
	void DDA_Line(int x1, int y1, int x2, int y2, Color penc);//΢�ַ���դ��������
	void bresenham(int x0, int y0, int x1, int y1, Color penc);
	void clerarCanvas(Layer* g_Layer);
	//����б��

	void drawPolygonline(PixelPoint* pts, int count, Color color);//��դ�������
	void drawCircle(double x, double y, double r, unsigned color);
	//Xɨ�����������
	void polygonScan(PixelPoint* pts, int count, Color penc);//�����Xɨ���߻��Ա߱����
	void polygonScan(Vertex3D* pts, int count, Color penc);//z_buffer,�����z���
	void drawPolygon(PixelPoint* data, int size, unsigned color);
	void clerarCanvas();
	
	//�����Xɨ���߻��Ա߱����
	static void MidPointClipAlgorithm(PixelPoint pt1, PixelPoint pt2, Layer* container);
	static void Sutherland_Hodgeman(PixelPoint rectPt1, PixelPoint rectPt2, Layer* container);
	void CohenSutherland(PixelPoint pt1, PixelPoint pt2, Layer* container);
	//�������
	void BoundaryFill(int x, int y, unsigned bdrColor, unsigned newColor);
	
	void PointFill(int x, int y, unsigned newColor);
	double ToGetDeepthZ(Vec4 pt0, Vec4 pt1, Vec4 pt2, int x, int y);
};
extern Raster g_Raster;