#pragma once
//#include "Matrix.h"
//#include "Vec.h"
#include "Graphic.h"
//#include "FractalImageGenerator.h"
#include"0_Vec4.h"
#define PMAXW 1200  //��Χ���
#define PMAXH 1200  //��Χ�߶�

const int INF = 9999;        //���ֵ

static Vec4 L(2, 2, 2);

class Base3DRender
{
public:
	// ���建����
	typedef struct gPixelBuc {
		Color color = 99;
		double depth = -9999;
	};

	gPixelBuc g_PixelBuc[PMAXW][PMAXH];

	// ���������������˽ṹ
	struct tpsFace {
		Vec4 point3D[3];
		int size = 3;
		Vec4 operator[](int i)
		{
			return point3D[i];
		}
	}CubeFace[12];//��������12����������ɵ���2*6=12

	
	double CubeScale = 1; // ��������
	double dx=0, dy=0, dz=0; // ƽ�ƾ���
	bool isLRtoUD = false; // �����ת�������»�����
	bool isCheckBack = false; // �Ƿ��������޳�
	Vec4 Xaxis; // �����ת��
	int texture_grid = 1; // �������������

	bool rotateLorE = true; // true�ƶ�����ӵ�

	Base3DRender();
	// ��ʼ���任����
	// transform = world * view *  proj
	void transform_init();
	// �任�������
	void transform_update();
	// ���ù۲�����ϵ
	void setCameraAtZero(Vec4 eye);
	void transform_normalize(Vec4& vec);

	// ����������
	void drawTraingle(Vec4& p1, Vec4& p2, Vec4& p3,Color color);
	//void drawTraingle(texture_v& p1, texture_v& p2, texture_v& p3);
	void drawTraingle(tpsFace& _cubeFace);

	// ������
	void drawPlane(int a, int b, int c, int d,Color color);

	// ����������
	void displayCube();
	void translate(double dx, double dy, double dz);
	void rotate(Vec4 axis, double angle);
	void Scale(double sX);

	// 3D�����������
	void eye_Rotate_Update(); // ������������
	void set_eye_Angle(double arc); // ���������ת�Ƕ�
	void set_eye_far(double dis); // �����������Զ��


	Vec4 getEyeCoord();
	// ����������
	void ptsConvertToFace(Vec4(&pts_)[8], tpsFace(&faces)[12]);

	// �����ƶ�����ӵ���߹�Դ
	void setRotateLight(bool isL);

	void CalLightIntensity(tpsFace(&faces)[12]);

	void setRGBv(double r, double g, double b);

	void reset3D();
private:
	Matrix4 mWorld;
	Matrix4 mView;
	Matrix4 mProj;
	
	double w = 600; // ��Ļ��
	double h = 600; // ��Ļ��
	Vec4 eye = {0,0,7};
	Matrix4 mTransform;
	Matrix4 mRotate; // ������ת����
	// �����嶥������


	




};
