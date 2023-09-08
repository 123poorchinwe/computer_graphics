#pragma once
#include"Vec3.h"
//�任����,��ʼΪ��λ����
struct Matrix4
{
	double _m[4][4];

	Matrix4() 
	{
		makeIdentity();
	}
	void makeIdentity()//��ʼ��
	{
		for (int i = 0; i < 4; ++i) 
		{
			for (int j = 0; j < 4; ++j)
				_m[i][j] = 0.0;
			_m[i][i] = 1.0;
		}
	}

	double* operator[](int i)
	{
		return _m[i];
	}

	Matrix4& makeTranslate(double dx, double dy, double dz);//ƽ��
	Matrix4& makeRotate(double rad,int i);//��ĳ����ת
	Matrix4& makeScale(double sx, double sy,double sz);//����
	Matrix4& makeMori();//��x,zƽ��Գ�
	Matrix4& makecut(double h);//��z�ᣬƫ��y�������
	
	static Matrix4 translate(double dx, double dy, double dz);
	static Matrix4 Rotate(double rad, int i);
	static Matrix4 scale(double sx, double sy, double sz);

	//��任
	static Matrix4 n_translate(double dx, double dy, double dz);
	//�۲����
	Matrix4&  makeLookAt(Vec3 eye, Vec3 center, Vec3 up);
	//͸��ͶӰ����
	Matrix4& makePerspective(double g_fovy, double aspectRatio, double g_zNear, double g_zFar);
	//����ͶӰ
	Matrix4& makeortho(double width, double height, double g_zNear, double g_zFar);
	
	//����*
	Matrix4 operator* (Matrix4& other)
	{
		Matrix4 m;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m._m[i][j] = 0;
				for (int k = 0; k < 4; k++) {
					m._m[i][j] += _m[i][k] * other._m[k][j];
					/*ÿһ��*ÿһ��*/
				}
			}
		}
		return m;
	}
	//����*=
	Matrix4& operator*= (Matrix4& other)
	{
		Matrix4 m = *this * other;
		*this = m;
		return *this;
	}

};

