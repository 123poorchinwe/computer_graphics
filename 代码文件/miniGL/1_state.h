#pragma once
#include "Graphic.h"
#include "GeoDefine3D.h"

enum DrawModeType { dmPixel, dmGrid };//绘制模式
enum DrawLineMethodType { dlDDA, dlMidBresenham };//光栅化线方法
enum DrawPolygonMethodType { dpXScanline, dp3DAET, dpBoundFill };//填充多边形方法
enum ClipMethodType { CohenSutherland, MidPoint, Sutherland_Hodgeman, };

enum State3D
{
	Blanking,
	Texture,
	LightSimple,
	LightGourand,
	NoneState,
};
struct State
{
	//绘制模式
	DrawModeType DrawMode;//绘制模式，dmPixel-像素模式， dmGrid-网格模式
	
	Color GridColor1;// 网格模式下颜色1
	Color GridColor2;//网格模式下颜色2
	Color GridWidth;//网格宽
	Color GridHeight; // 网格高

	// 画线方法
	DrawLineMethodType DrawLineMethod;//dlDDA--DDA,dlMidBresenham --MidBresenham
	
	FillMode Fi11Mode; // 填充模式
	//填充多边形方法
	DrawPolygonMethodType DrawPolygonMethod;//dpXScanline-X扫描线活性边表，dp3DAET-活性边表三维，dpBoundFill-种子点填充


	// 裁剪算法
	ClipMethodType ClipMethod;
	// 边表节点
	struct tagEDGE {
		double x;
		double dx;
		int ymax;
	};

	State3D g_3DState;
	CULLMode CullMode;// 剔除模式

	State();//初始化参数
	
};
extern State g_State;
