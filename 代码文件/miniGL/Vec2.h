#pragma once
#include <cmath>

struct Vec2
{
	Vec2()
	{
		_v[0] = _v[1] = 0.0;
	}

	Vec2(double x, double y)
	{
		_v[0] = x;
		_v[1] = y;
		
	}

	void set(double x, double y)
	{
		_v[0] = x;
		_v[1] = y;
		
	}

	double& x() { return _v[0]; }
	double& y() { return _v[1]; }
	
	double& operator[](int i)
	{
		return _v[i];
	}

	//向量叉乘^重载
	double operator^(Vec2& u)
	{
		double v;

		v= _v[0] * u[1] - _v[1] * u[0];
		
		return v;
	}

	//向量点积*重载，结果为一标量
	double operator*(Vec3& u)
	{
		//Vec3 v;
		double b;
		b = _v[0] * u[0] + _v[1] * u[1] + _v[2] * u[2];

		return b;
	}

	

	double _v[2];
};