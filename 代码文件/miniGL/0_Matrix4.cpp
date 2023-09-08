#define _CRT_SECURE_NO_WARNINGS 1
#include "0_Matrix4.h"
#include <math.h>
#include "Vec3.h"
#include "Define.h"
#include"0_Vec4.h"
//构造平移矩阵
Matrix4& Matrix4::makeTranslate(double dx, double dy, double dz)
{
    makeIdentity();
    _m[3][0] = dx;
    _m[3][1] = dy;
    _m[3][2] = dz;
    return *this;
}
//构造沿某个轴的旋转矩阵
Matrix4& Matrix4::makeRotate(double rad,int i)
{
    makeIdentity();
    double _cos = cos(rad);
    double _sin = sin(rad);
    if (i == 0)
    {
        _m[1][1] = _cos;
        _m[1][2] = _sin;
        _m[2][1] = -_sin;
        _m[2][2] = _cos;
        return *this;
    }
    else if (i == 1)
    {
        _m[0][0] = _cos;
        _m[0][2] = -_sin;
        _m[2][0] = _sin;
        _m[2][2] = _cos;
        return *this;
    }
    else if (i == 2)
    {
        _m[0][0] = _cos;
        _m[0][1] = _sin;
        _m[1][0] = -_sin;
        _m[1][1] = _cos;
        return *this;
    }
    
}
//等比例缩放矩阵
Matrix4& Matrix4::makeScale(double sx, double sy,double sz)
{
    makeIdentity();

    _m[0][0] = sx;
    _m[1][1] = sy;
    _m[2][2] = sz;
    return *this;
}
//与x,z平面对称
Matrix4& Matrix4::makeMori()
{
    makeIdentity();

    _m[0][0] = 1;
    _m[1][1] = -1;
    _m[2][2] = 1;
    return *this;
}
//沿z方向，偏离y轴的错切
Matrix4& Matrix4::makecut(double h)
{
    makeIdentity();

    _m[1][2] = h;

    return *this;
}

//平移
Matrix4 Matrix4::translate(double dx, double dy, double dz)
{
    Matrix4 m;
    m.makeTranslate(dx, dy,dz);
    return m;
}

//旋转
Matrix4 Matrix4::Rotate(double rad, int i)
{
    Matrix4 m;
    m.makeRotate(rad,i);
    return m;
}

//放缩
Matrix4 Matrix4::scale(double sx, double sy,double sz)
{
    Matrix4 m;
    m.makeScale(sx, sy,sz);
    return m;
}
//平移的逆矩阵
Matrix4 Matrix4::n_translate(double dx, double dy, double dz)
{
    Matrix4 m;
    m.makeTranslate(-dx, -dy, -dz);
    return m;
}

//观察矩阵
Matrix4& Matrix4::makeLookAt(Vec3 eye, Vec3 center, Vec3 up)
{
    makeIdentity();
    Vec3 x, y, z;
    z = (eye - center).normalize();
    //x = (up.cross(up, z)).normalize();
    x = up ^ z;
    x = (up^ z).normalize();
    y = z ^ x;
    y = y.normalize();
    for (int i = 0; i < 3; ++i)
    {
        _m[i][0] = x[i];
    }
    for (int i = 0; i < 3; ++i)
    {
        _m[i][1] = y[i];
    }
    for (int i = 0; i < 3; ++i)
    {
        _m[i][2] = z[i];
    }

    _m[3][0] = -(eye*x);
    _m[3][1] = -(eye * y);
    _m[3][2] = -(eye * z);
    return *this;

}
//透视投影矩阵
Matrix4& Matrix4::makePerspective(double g_fovy, double aspectRatio, double g_zNear, double g_zFar)
{
    makeIdentity();
    //g_fovy *= 3.14159265 / 180;
     g_fovy = DEG_RAD(g_fovy);
    _m[0][0] = 1.0 / (aspectRatio * tan(g_fovy / 2));
    _m[1][1] = 1.0 / tan(g_fovy / 2);
    _m[2][2] = g_zFar / (g_zNear - g_zFar);
    _m[2][3] = -1.0;
    _m[3][2] = g_zNear * g_zFar / (g_zNear - g_zFar);
    _m[3][3] = 0;
    return *this;

}
//正交投影
Matrix4& Matrix4::makeortho(double width, double height, double g_zNear, double g_zFar)
{
    makeIdentity();
    _m[0][0] = 2.0/ width;
    _m[1][1] = 2.0 / height;
    _m[2][2] = -1.0 / (  g_zFar- g_zNear);
    
    _m[3][2] = -g_zNear  / ( g_zFar-g_zNear  );
    return *this;

}



