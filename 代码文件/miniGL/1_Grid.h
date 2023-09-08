#pragma once
#include"Graphic.h"
struct Grid
{
	//背景网格绘制
	void drawGrid();

	//将像素坐标用网格行列号绘制
	int pixelToGrid(int x0, int y0, int& x1, int& y1);//像素坐标转为网格行列号
	void drawCell(int x0, int y0, Color color);//画网格

};
extern Grid g_Grid;