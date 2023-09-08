#pragma once
#include "Graphic.h"
#include "GeoDefine3D.h"

enum DrawModeType { dmPixel, dmGrid };//����ģʽ
enum DrawLineMethodType { dlDDA, dlMidBresenham };//��դ���߷���
enum DrawPolygonMethodType { dpXScanline, dp3DAET, dpBoundFill };//������η���
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
	//����ģʽ
	DrawModeType DrawMode;//����ģʽ��dmPixel-����ģʽ�� dmGrid-����ģʽ
	
	Color GridColor1;// ����ģʽ����ɫ1
	Color GridColor2;//����ģʽ����ɫ2
	Color GridWidth;//�����
	Color GridHeight; // �����

	// ���߷���
	DrawLineMethodType DrawLineMethod;//dlDDA--DDA,dlMidBresenham --MidBresenham
	
	FillMode Fi11Mode; // ���ģʽ
	//������η���
	DrawPolygonMethodType DrawPolygonMethod;//dpXScanline-Xɨ���߻��Ա߱�dp3DAET-���Ա߱���ά��dpBoundFill-���ӵ����


	// �ü��㷨
	ClipMethodType ClipMethod;
	// �߱�ڵ�
	struct tagEDGE {
		double x;
		double dx;
		int ymax;
	};

	State3D g_3DState;
	CULLMode CullMode;// �޳�ģʽ

	State();//��ʼ������
	
};
extern State g_State;
