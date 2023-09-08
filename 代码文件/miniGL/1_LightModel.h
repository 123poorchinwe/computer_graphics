#pragma once
#include "Graphic.h"
//#include "Vec.h"
#include "Base3DRenderer.h"
//#include "Matrix.h"
#include"0_Vec4.h"
#include <vector>
#include <algorithm>
#include <minwindef.h>

//=======================================局部光照模型部分=========================================

	// 定义恒定光强下的每个三角形面的结构体
struct Lightface {
	int polyID;   // 编号
	double A, B, C, D;  //面方程x+By+Cz+D=0的系数
	Vec4 spmv;   //面的法向量
	Color color;   //面的颜色
	Vec4 points[3]; //三角面的三个顶点坐标存储
};

// newBuffer用来存储光照模型下每个像素的颜色值
//vector<vector<Color>> newBuffer;

// rgb光照模型参数
// 
// 环境光 ambient light
double Iar = 0.2, Iag = 0.2, Iab = 0.2;
// 入射光Ip
double Ipr = 0.5, Ipg = 0.5, Ipb = 0.5;

// 环境光反射系数 Ienv=KaIa
double Kar = 0.1, Kag = 0.1, Kab = 0.1;

// 漫反射 diffuse reflection
// Lambert漫反射模型
// n·I为入射角度
// r为光源与照射点的距离
// Ld=Kd(I/r*r)max(0,n·I)
// 漫反射系数 Kd 
double Kdr = (0.8 * 300), Kdg = (0.8 * 300), Kdb = (0.8 * 300);

// 镜面反射光 specular hightlights
// 镜面反射系数 Ks
// 考虑观察者的视角
// Ls=Ks(I/r*r)max(0,cosα)P
double Ksr=0.5, Ksg=0.5, Ksb=0.5;

//============================================================================================
//=========================高洛德光照模型部分======================================
struct rgb
{
	double r, g, b;
};
struct PtVector
{
	int polyID;   //该顶点所属的多边形
	int indexID;  //点在这个面中的顺序的索引
	Vec4 point;
	Vec4 vec;
	double light;   //这个点最后的光照强度
	rgb RGB;
	int lightcolor;
};

struct GrdFaces {
	int polyID;
	double A, B, C, D;
	Vec4 spmv;
	Color color;  //每个面的颜色
	PtVector points[3];   //一个面的三个顶点
	int size = 3;  //点数量
};
GrdFaces groundFace[12];

double GetDistance(Vec4& Light, Vec4& Targrt)
{
	double d1 = sqrt(pow(Targrt.x() - Light.x(), 2) + pow(Targrt.y() - Light.y(), 2) + pow(Targrt.z() - Light.z(), 2));
	//double d2 = sqrt(pow(Targrt.x - eye.x, 2) + pow(Targrt.y - eye.y, 2) + pow(Targrt.z - eye.z, 2));
	return d1;
}
Vec4 getReflect(Vec4 light, Vec4 norm)
{

	//计算公式：R=I-2(I·N)N

	Vec4 reflect;
	double K = 2 * light .vector_dotMutiply(norm);

	Vec4 temp;
	temp.x() = K * norm.x();
	temp.y() = K * norm.y();
	temp.z() = K * norm.z();
	temp.w() = K * norm.w();

	reflect = light - temp;
	return reflect;
}

void GetABC(Base3DRender::tpsFace& face, double& a, double& b, double& c, double& d)
{

	Vec4 pt0, pt1, pt2;
	pt0 = face.point3D[0];
	pt1 = face.point3D[1];
	pt2 = face.point3D[2];
	double mx = pt1.x() - pt0.x();
	double my = pt1.y() - pt0.y();
	double mz = pt1.z() - pt0.z();

	double nx = pt2.x() - pt0.x();
	double ny = pt2.y() - pt0.y();
	double nz = pt2.z() - pt0.z();
	a = my * nz - ny * mz;
	b = nx * mz - mx * nz;
	c = mx * ny - nx * my;

	d = -(a * pt0.x() + b * pt0.y() + c * pt0.z());
}
bool SpMVdrector(Vec4 m, Vec4 n)
{
	//判断2个向量方向是否同向
	//同向 返回为true 否则 返回为false
	if ((m.x() * n.x() + m.y() * n.y() + m.z() * n.z()) > 0)
		return true;
	return false;
}
Lightface getLightFace(Base3DRender::tpsFace& faces)
{

	Lightface newLightface;
	/*newLightface.polyID = i;
	newLightface.color = i;*/
	//将三角面三个顶点的信息存储到需要研究的光照模型中去
	//将face[i]里的信息存储到Lightface[i]中

	// 存储面的ABCD信息
	GetABC(faces, newLightface.A, newLightface.B, newLightface.C, newLightface.D);
	// 存储面的点信息
	int pointsize = 3;
	for (int j = 0; j < pointsize; j++)
	{
		newLightface.points[j] = faces.point3D[j];
	}
	// 存储面的法向量信息
	newLightface.spmv.x() = newLightface.A;
	newLightface.spmv.y() = newLightface.B;
	newLightface.spmv.z() = newLightface.C;

	// 定义一个从图形中心指向图形面的顶点的向量
	Vec4 center(0, 0, 0);
	Vec4 tempVec = center - newLightface.points[0];

	// 使得所有法向量指向正方体外部
	if (!SpMVdrector(newLightface.spmv, tempVec))
	{
		newLightface.spmv.x() *= -1;
		newLightface.spmv.y() *= -1;
		newLightface.spmv.z() *= -1;
	}
	return newLightface;
}

double GetZdeep(double A, double B, double C, double D, double x, double y)
{
	if (C == 0)
	{
		return -9999;
	}

	//如果不是
	return (A * x + B * y + D) / (-C);
}


double ToGetLightFaces(double x, double y, Lightface& lface, Color gcolor,Vec4& eye)
{
	Iar = Iag = Iab = 0.2;
	Ipr = Ipg = Ipb = 0.5;

	Kar = Kag = Kab = 0.1;
	Kdr = Kdg = Kdb = 0.8 * 300; // 太小了，乘以一个倍数放大一下效果
	Ksr = Ksg = Ksb = 0.5;

	// 对光赋值
	//vector3D Light(0, 0, 1);
	Vec4 Light = L;
	double Ia, Id, Is;

	// 1、环境光
	Ia = Kar * Iar;

	// 2、漫反射
	// 法向量
	Vec4 norm = lface.spmv;
	norm.vector_normalize();

	double z = GetZdeep(lface.A, lface.B, lface.C, lface.D, x, y);
	// 反射点位置
	Vec4 fragPos(x, y, z);
	// 光线方向
	// Light为光源坐标
	Vec4 lightDir = Light - fragPos;
	lightDir.vector_normalize();

	float diff = norm.vector_dotMutiply(lightDir) > 0 ? norm.vector_dotMutiply(lightDir) : 0;
	// 漫反射应该还要乘一个距离比例系数r*r，但是效果不太好
	/*double distance = sqrt(pow(norm.x - fragPos.x, 2) + pow(norm.y - fragPos.y, 2));
	double r = distance > 1 ? distance : 1;*/
	Id = diff * Kdr * Ipr;

	// 3、镜面反射
	//vector3D viewPos(260, 220, -3.5);
	Vec4 viewPos = eye * 10;
	Vec4 viewDir = viewPos - fragPos;
	viewDir.vector_normalize();
	Vec4 reflectDir = getReflect(lightDir, norm);
	double dot = viewDir.vector_dotMutiply(reflectDir) > 0 ? viewDir.vector_dotMutiply(reflectDir) : 0;
	// 高光的反光度(Shininess)
	//一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
	int shininess = 3;
	float spec = pow(dot, shininess);
	double specLight = 1;
	Is = spec * Ksr * specLight;

	double I = Id + Ia + Is;
	return I;
}

GrdFaces getGrdFace(Base3DRender::tpsFace& face)
{
	GrdFaces groundFace;

	//将三角面三个顶点的信息存储到需要研究的光照模型中去
	//将face[i]里的信息存储到groundface[i]中

	// 存储面的ABCD信息
	GetABC(face, groundFace.A, groundFace.B, groundFace.C, groundFace.D);

	// 存储面的点信息
	int pointsize = 3;
	for (int j = 0; j < pointsize; ++j)
	{
		//groundFace.points[j].polyID = i;	 //每一个顶点所属的多边形
		groundFace.points[j].indexID = j; //获取这个点在面中的索引下标
		groundFace.points[j].point = face.point3D[j];
	}

	//存储面的法向量信息
	groundFace.spmv.x() = groundFace.A;
	groundFace.spmv.y() = groundFace.B;
	groundFace.spmv.z() = groundFace.C;

	// 使得所有法向量指向正方体外部
	// 定义一个从图形中心指向图形面的顶点的向量
	Vec4 center(0, 0, 0);
	Vec4 tempVec = center - groundFace.points[0].point;
	if (!SpMVdrector(groundFace.spmv, tempVec))
	{
		groundFace.spmv.x() *= -1;
		groundFace.spmv.y() *= -1;
		groundFace.spmv.z() *= -1;
	}

	//得到每个顶点在每一个面的法向量
	//并将顶点处的法向量单位化
	for (int j = 0; j < 3; ++j)
	{
		groundFace.points[j].vec = groundFace.spmv;
		groundFace.points[j].vec.vector_normalize();
	}
	return groundFace;
}


double ToGetGroundFaces(double x, double y, Color pixel, Color ColorMode, Vec4 Light, Vec4& tarv, Vec4& eye)
{
	Kdr = Kdg = Kdb = 0.8 * 300;
	double Ia, Id, Is;
	// 1、环境光
	Ia = Kar * Iar;
	// 2、漫反射
	// 法向量
	Vec4 norm = tarv;
	//norm.vector_normalize();

	//求解点光源到像素坐标点的向量
	//首先求解面上x,y的三维空间向量
	double z = GetZdeep(groundFace[pixel].A, groundFace[pixel].B, groundFace[pixel].C, groundFace[pixel].D, x, y);
	// 反射点位置
	Vec4 fragPos(x, y, z);

	// 光线方向
	// Light为光源坐标
	Vec4 lightDir = Light - fragPos;
	lightDir.vector_normalize();
	float diff = norm.vector_dotMutiply(lightDir) > 0 ? norm.vector_dotMutiply(lightDir) : 0;
	// 漫反射应该还要乘一个距离比例系数r*r，但是效果不太好
	/*double distance = sqrt(pow(norm.x - fragPos.x, 2) + pow(norm.y - fragPos.y, 2));
	double r = distance > 1 ? distance : 1;*/
	Id = diff * Kdr * Ipr;
	// 3、镜面反射
	//vector3D viewPos(260, 220, -0.5);
	Vec4 viewPos = eye * 10;
	Vec4 viewDir = viewPos - fragPos;
	viewDir.vector_normalize();
	Vec4 reflectDir = getReflect(lightDir, norm);
	double dot = viewDir.vector_dotMutiply(reflectDir) > 0 ? viewDir.vector_dotMutiply(reflectDir) : 0;
	// 高光的反光度(Shininess)
	//一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
	int shininess = 3;
	float spec = pow(dot, shininess);
	double specLight = 1;
	Is = spec * Ksr * specLight;

	/*double c0 = 0.8;
	double c1 = 0.0000001;
	double c2 = 0.000000001;
	double distance = GetDistance(Light, fragPos) + GetDistance(fragPos,viewPos);
	double disDecay = 1 / (c0 + c1 * distance + c2 * pow(distance, 2));
	double fd = min(1.0, distance);
	Id *= fd;
	Is *= fd;*/

	double I = Id + Ia + Is;
	return I;
}


double ToGetGroundFaces(double x, double y,  double z, Vec4 Light, Vec4& tarv, Vec4& eye, Matrix4& m)
{
	Kdr = Kdg = Kdb = 0.2;
	double Ia, Id, Is;
	// 1、环境光
	Ia = Kar * Iar;
	// 2、漫反射
	// 法向量
	Vec4 norm = tarv;
	norm.vector_normalize();
	// 反射点位置
	Vec4 fragPos(x, y, z);
	fragPos = fragPos.vectorMultiplyMatrix3D(fragPos, m);
	//fragPos = fragPos * (1.0 / fragPos.h);

	// 光线方向
	// Light为光源坐标
	Vec4 lightDir = Light - fragPos;
	lightDir.vector_normalize();
	float diff = lightDir.vector_dotMutiply(norm) > 0 ? lightDir.vector_dotMutiply(norm) : 0;
	// 漫反射应该还要乘一个距离比例系数r*r，但是效果不太好
	/*double distance = sqrt(pow(norm.x - fragPos.x, 2) + pow(norm.y - fragPos.y, 2));
	double r = distance > 1 ? distance : 1;*/
	Id = diff * Kdr * Ipr;
	// 3、镜面反射
	//vector3D viewPos(260, 220, -0.5);
	Vec4 viewPos = eye;
	Vec4 viewDir = viewPos - fragPos;
	viewDir.vector_normalize();
	Vec4 reflectDir = getReflect(lightDir*(-1), norm);
	double dot = reflectDir.vector_dotMutiply(viewDir) > 0 ? reflectDir.vector_dotMutiply(viewDir) : 0;
	// 高光的反光度(Shininess)
	//一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
	Vec4 H = (lightDir + viewDir) * 0.5;
	H.vector_normalize();
	int shininess = 1;
	float spec = sqrt(pow(dot, shininess));
	//Is = spec * Ksr * specLight;
	Is = spec * Ksr * Ipr;

	double c0 = 0.8;
	double c1 = 0.0000001;
	double c2 = 0.000000001;
	double distance = lightDir.getVectorLength()+viewDir.getVectorLength();
	double disDecay = 1 / (c0 + c1 * distance + c2 * pow(distance, 2));
	double fd = min(1.0, disDecay);
	Id *= fd;
	Is *= fd;

	double I = Id + Ia + Is;
	return I;
}











