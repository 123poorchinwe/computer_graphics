#pragma once
#include"Graphic.h"
struct Grid
{
	//�����������
	void drawGrid();

	//�������������������кŻ���
	int pixelToGrid(int x0, int y0, int& x1, int& y1);//��������תΪ�������к�
	void drawCell(int x0, int y0, Color color);//������

};
extern Grid g_Grid;