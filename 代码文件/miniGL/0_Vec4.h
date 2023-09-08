#pragma once
#include "0_Matrix4.h"
#include "Vec3.h"
struct Vec4
{
	double _v[4];
	Vec4()
	{
		_v[0] = _v[1] = _v[2] = _v[3] = 0.0;
	}

	Vec4(double x, double y, double z,double w)
	{
		_v[0] = x;
		_v[1] = y;
		_v[2] = z;
		_v[3] = w;
	}

	Vec4(double x, double y,double z)
	{
		_v[0] = x;
		_v[1] = y;
		_v[2] = z;
		_v[3] = 1.0;
	}

	Vec4(Vec3& v)
	{
		_v[0] = v[0];
		_v[1] = v[1];
		_v[2] = v[2];
		_v[3] = 1.0;
		//set(v.x, v.y, v.z, 1.0);
	}

	void set(double x, double y, double z,double w)
	{
		_v[0] = x;
		_v[1] = y;
		_v[2] = z;
		_v[3] = w;
	}

	double& x() { return _v[0]; }
	double& y() { return _v[1]; }
	double& z() { return _v[2]; }
	double& w() { return _v[3]; }

	double& operator[](int i)
	{
		return _v[i];
	}
	//重载向量*矩阵
	Vec4 operator*(Matrix4& m)
	{
		Vec4 v;
		for (int i = 0; i < 4; i++) {
			v[i] = _v[0] * m[0][i] + _v[1] * m[1][i] + _v[2] * m[2][i] +_v[3] * m[3][i];
		}
		return v;
	}
	//重载*=
	Vec4& operator*=(Matrix4& m)
	{
		Vec4 v = *this * m;
		*this = v;
		return *this;
	}

	// 向量叉乘
	Vec4 vector_crossMutiply(Vec4& pt)
	{
		Vec4 result;
		result.x() = this->y() * pt.z() - this->z() * pt.y();
		result.y() = this->z() * pt.x() - this->x() * pt.z();
		result.z() = this->x() * pt.y() - this->y() * pt.x();
		return result;
	}

	// 归一化
	void vector_normalize()
	{
		double len = this->getVectorLength();
		if (len != 0)
		{
			double temp = 1.0 / len;
			this->x() = this->x() * temp;
			this->y() = this->y() * temp;
			this->z() = this->z() * temp;
		}
	}

	// 获取向量长度
	double getVectorLength()
	{
		return sqrt(this->x() * this->x() + this->y() * this->y() + this->z() * this->z());
	}

	//点乘
	Vec4 vectorMultiplyMatrix3D(Vec4& vec, Matrix4& m)
	{
		Vec4 result;

		result.x() = vec.x() * m[0][0] + vec.y() * m[1][0] + vec.z() * m[2][0] + vec.w() * m[3][0];
		result.y() = vec.x() * m[0][1] + vec.y() * m[1][1] + vec.z() * m[2][1] + vec.w() * m[3][1];
		result.z() = vec.x() * m[0][2] + vec.y() * m[1][2] + vec.z() * m[2][2] + vec.w() * m[3][2];
		result.w() = vec.x() * m[0][3] + vec.y() * m[1][3] + vec.z() * m[2][3] + vec.w() * m[3][3];

		return result;
	}
	// 向量点乘
	double vector_dotMutiply(Vec4& pt)
	{
		return this->x() * pt.x() + this->y() * pt.y() + this->z() * pt.z();
	}
	//向量减
	Vec4 operator-( Vec4& pt)
	{
		Vec4 NewVec3D;
		NewVec3D.x() = x() - pt.x();
		NewVec3D.y() = y() - pt.y();
		NewVec3D.z() = z() - pt.z();
		return NewVec3D;
	}
	//向量乘
	Vec4 operator*(const double& fv)
	{
		Vec4 NewVec3D;
		NewVec3D.x() = x() * fv;
		NewVec3D.y() = y() * fv;
		NewVec3D.z() = z() * fv;
		return NewVec3D;
	}
	//向量加
	Vec4 operator+( Vec4& pt)
	{
		Vec4 NewVec3D;
		NewVec3D.x() = x() + pt.x();
		NewVec3D.y() = y() + pt.y();
		NewVec3D.z() = z() + pt.z();
		return NewVec3D;
	}
};

