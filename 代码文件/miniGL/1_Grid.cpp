#include"1_Grid.h"
#include <stdlib.h>
#include"Graphic.h"
#include "GeoDefine.h"
#include"1_state.h"

Grid g_Grid;

//全屏幕按网格行列号绘制
void Grid::drawGrid()
{
	int x0, y0, x1, y1;
	DPtToLPt(0, 0, x0, y0);
	DPtToLPt(getWindowWidth() - 1, getWindowHeight() - 1, x1, y1);


	g_Grid.pixelToGrid(x0, y0, x0, y0);
	g_Grid.pixelToGrid(x1, y1, x1, y1);
	if (y0 > y1)
		std::swap(y0, y1);

	for (int col = x0; col <= x1; ++col)
		for (int row = y0; row <= y1; ++row)
			g_Grid.drawCell(col, row, (col + row) % 2 ? g_State.GridColor1 : g_State.GridColor2);

}

//求像素所在的行列号
int Grid::pixelToGrid(int x0, int y0, int& x1, int& y1)
{
	double w = 1.0 / g_State.GridWidth;
	double h = 1.0 / g_State.GridHeight;
	x1 = x0 * w;

	y1 = y0 * h;

	return  x1, y1;
}

//根据行列号，绘制网格
void Grid::drawCell(int col, int row, Color color)
{
	int x0 = col * g_State.GridWidth, x1 = x0 + g_State.GridWidth - 1;
	int y0 = row * g_State.GridHeight, y1 = y0 + g_State.GridHeight - 1;

	LPtToDPt(x0, y0, x0, y0);
	LPtToDPt(x1, y1, x1, y1);

	if (y0 > y1)
		std::swap(y0, y1);
	for (int i = y0; i <= y1; i++)
		for (int j = x0; j <= x1; j++)
		{
			setDevicePixel(j, i, color);
		}
}