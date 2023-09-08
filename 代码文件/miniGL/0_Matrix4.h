#pragma once
#include"Vec3.h"
//变换矩阵,初始为单位矩阵
struct Matrix4
{
	double _m[4][4];

	Matrix4() 
	{
		makeIdentity();
	}
	void makeIdentity()//初始化
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

	Matrix4& makeTranslate(double dx, double dy, double dz);//平移
	Matrix4& makeRotate(double rad,int i);//沿某轴旋转
	Matrix4& makeScale(double sx, double sy,double sz);//缩放
	Matrix4& makeMori();//以x,z平面对称
	Matrix4& makecut(double h);//沿z轴，偏离y方向错切
	
	static Matrix4 translate(double dx, double dy, double dz);
	static Matrix4 Rotate(double rad, int i);
	static Matrix4 scale(double sx, double sy, double sz);

	//逆变换
	static Matrix4 n_translate(double dx, double dy, double dz);
	//观察矩阵
	Matrix4&  makeLookAt(Vec3 eye, Vec3 center, Vec3 up);
	//透视投影矩阵
	Matrix4& makePerspective(double g_fovy, double aspectRatio, double g_zNear, double g_zFar);
	//正交投影
	Matrix4& makeortho(double width, double height, double g_zNear, double g_zFar);
	
	//重载*
	Matrix4 operator* (Matrix4& other)
	{
		Matrix4 m;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m._m[i][j] = 0;
				for (int k = 0; k < 4; k++) {
					m._m[i][j] += _m[i][k] * other._m[k][j];
					/*每一行*每一列*/
				}
			}
		}
		return m;
	}
	//重载*=
	Matrix4& operator*= (Matrix4& other)
	{
		Matrix4 m = *this * other;
		*this = m;
		return *this;
	}

};

