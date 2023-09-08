#pragma once
#include "Matrix3.h"
#include <cmath>
struct Vec3
{
	Vec3 operator+(Vec3& v)
	{
		Vec3 t;
		for (int i = 0; i < 3; ++i)
		{
			t[i] = _v[i] + v[i];
		}
		return t;
	}
	void operator*=(double s) //标量乘法
	{
		for (int i = 0; i < 3; ++i)
		{
			_v[i] *= s;
		}
	}
	Vec3 reflect(Vec3 v1, Vec3 n)
	{
		Vec3 v0;
		double temp = v1 * n;
		Vec3 t = n;
		t *= temp;
		t *= 2;
		return v0 = v1 - t;
	}
	Vec3()
	{
		_v[0] = _v[1] = _v[2] = 0.0;
	}

	Vec3(double x, double y, double z)
	{
		_v[0] = x;
		_v[1] = y;
		_v[2] = z;
	}

	Vec3(double x, double y)
	{
		_v[0] = x;
		_v[1] = y;
		_v[2] = 1.0;
	}

	void set(double x, double y, double z)
	{
		_v[0] = x;
		_v[1] = y;
		_v[2] = z;
	}

	double& x() { return _v[0]; }
	double& y() { return _v[1]; }
	double& z() { return _v[2]; }



	double& operator[](int i)
	{
		return _v[i];
	}

	//向量与矩阵相乘的运算符重载
	Vec3 operator*(Matrix3& m)
	{
		Vec3 v;
		for (int i = 0; i < 3; i++) {
			v[i] = _v[0] * m[0][i] + _v[1] * m[1][i] + _v[2] * m[2][i];
		}
		return v;
	}
	
	//*=重载
	Vec3& operator*=(Matrix3& m)
	{
		Vec3 v = *this * m;
		*this = v;
		return *this;
	}

	//向量相减-运算符重载
	Vec3 operator-(Vec3& u)
	{
		Vec3 v;
		
		v[0] = _v[0] - u[0];
		v[1] = _v[1] - u[1];
		v[2] = _v[2] - u[2];
		return v;
	}

	//向量叉乘^重载
	Vec3 operator^(Vec3& u)
	{
		Vec3 v;

		v[0] = _v[1] * u[2]- _v[2] * u[1];
		v[1] = _v[2] * u[0] - _v[0] * u[2];
		v[2] = _v[0] * u[1] - _v[1] * u[0];
		return v;
	}

	//向量点积*重载，结果为一标量
	double operator*(Vec3& u)
	{
		//Vec3 v;
		double b;
		b = _v[0] * u[0]+ _v[1] * u[1]+_v[2] * u[2];
		
		return b;
	}

	//初始化
	Vec3 normalize()
	{
		Vec3 v;
		double len = 1.0/sqrt(_v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2]);
		v[0] = _v[0] * len;
		v[1] = _v[1] * len;
		v[2] = _v[2] * len;
		return v;
	}

	

	double _v[3];
};

