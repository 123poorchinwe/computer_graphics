#pragma once
#include "Graphic.h"
//#include "Vec.h"
#include "Base3DRenderer.h"
//#include "Matrix.h"
#include"0_Vec4.h"
#include <vector>
#include <algorithm>
#include <minwindef.h>

//=======================================�ֲ�����ģ�Ͳ���=========================================

	// ����㶨��ǿ�µ�ÿ����������Ľṹ��
struct Lightface {
	int polyID;   // ���
	double A, B, C, D;  //�淽��x+By+Cz+D=0��ϵ��
	Vec4 spmv;   //��ķ�����
	Color color;   //�����ɫ
	Vec4 points[3]; //�������������������洢
};

// newBuffer�����洢����ģ����ÿ�����ص���ɫֵ
//vector<vector<Color>> newBuffer;

// rgb����ģ�Ͳ���
// 
// ������ ambient light
double Iar = 0.2, Iag = 0.2, Iab = 0.2;
// �����Ip
double Ipr = 0.5, Ipg = 0.5, Ipb = 0.5;

// �����ⷴ��ϵ�� Ienv=KaIa
double Kar = 0.1, Kag = 0.1, Kab = 0.1;

// ������ diffuse reflection
// Lambert������ģ��
// n��IΪ����Ƕ�
// rΪ��Դ�������ľ���
// Ld=Kd(I/r*r)max(0,n��I)
// ������ϵ�� Kd 
double Kdr = (0.8 * 300), Kdg = (0.8 * 300), Kdb = (0.8 * 300);

// ���淴��� specular hightlights
// ���淴��ϵ�� Ks
// ���ǹ۲��ߵ��ӽ�
// Ls=Ks(I/r*r)max(0,cos��)P
double Ksr=0.5, Ksg=0.5, Ksb=0.5;

//============================================================================================
//=========================����¹���ģ�Ͳ���======================================
struct rgb
{
	double r, g, b;
};
struct PtVector
{
	int polyID;   //�ö��������Ķ����
	int indexID;  //����������е�˳�������
	Vec4 point;
	Vec4 vec;
	double light;   //��������Ĺ���ǿ��
	rgb RGB;
	int lightcolor;
};

struct GrdFaces {
	int polyID;
	double A, B, C, D;
	Vec4 spmv;
	Color color;  //ÿ�������ɫ
	PtVector points[3];   //һ�������������
	int size = 3;  //������
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

	//���㹫ʽ��R=I-2(I��N)N

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
	//�ж�2�����������Ƿ�ͬ��
	//ͬ�� ����Ϊtrue ���� ����Ϊfalse
	if ((m.x() * n.x() + m.y() * n.y() + m.z() * n.z()) > 0)
		return true;
	return false;
}
Lightface getLightFace(Base3DRender::tpsFace& faces)
{

	Lightface newLightface;
	/*newLightface.polyID = i;
	newLightface.color = i;*/
	//�������������������Ϣ�洢����Ҫ�о��Ĺ���ģ����ȥ
	//��face[i]�����Ϣ�洢��Lightface[i]��

	// �洢���ABCD��Ϣ
	GetABC(faces, newLightface.A, newLightface.B, newLightface.C, newLightface.D);
	// �洢��ĵ���Ϣ
	int pointsize = 3;
	for (int j = 0; j < pointsize; j++)
	{
		newLightface.points[j] = faces.point3D[j];
	}
	// �洢��ķ�������Ϣ
	newLightface.spmv.x() = newLightface.A;
	newLightface.spmv.y() = newLightface.B;
	newLightface.spmv.z() = newLightface.C;

	// ����һ����ͼ������ָ��ͼ����Ķ��������
	Vec4 center(0, 0, 0);
	Vec4 tempVec = center - newLightface.points[0];

	// ʹ�����з�����ָ���������ⲿ
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

	//�������
	return (A * x + B * y + D) / (-C);
}


double ToGetLightFaces(double x, double y, Lightface& lface, Color gcolor,Vec4& eye)
{
	Iar = Iag = Iab = 0.2;
	Ipr = Ipg = Ipb = 0.5;

	Kar = Kag = Kab = 0.1;
	Kdr = Kdg = Kdb = 0.8 * 300; // ̫С�ˣ�����һ�������Ŵ�һ��Ч��
	Ksr = Ksg = Ksb = 0.5;

	// �Թ⸳ֵ
	//vector3D Light(0, 0, 1);
	Vec4 Light = L;
	double Ia, Id, Is;

	// 1��������
	Ia = Kar * Iar;

	// 2��������
	// ������
	Vec4 norm = lface.spmv;
	norm.vector_normalize();

	double z = GetZdeep(lface.A, lface.B, lface.C, lface.D, x, y);
	// �����λ��
	Vec4 fragPos(x, y, z);
	// ���߷���
	// LightΪ��Դ����
	Vec4 lightDir = Light - fragPos;
	lightDir.vector_normalize();

	float diff = norm.vector_dotMutiply(lightDir) > 0 ? norm.vector_dotMutiply(lightDir) : 0;
	// ������Ӧ�û�Ҫ��һ���������ϵ��r*r������Ч����̫��
	/*double distance = sqrt(pow(norm.x - fragPos.x, 2) + pow(norm.y - fragPos.y, 2));
	double r = distance > 1 ? distance : 1;*/
	Id = diff * Kdr * Ipr;

	// 3�����淴��
	//vector3D viewPos(260, 220, -3.5);
	Vec4 viewPos = eye * 10;
	Vec4 viewDir = viewPos - fragPos;
	viewDir.vector_normalize();
	Vec4 reflectDir = getReflect(lightDir, norm);
	double dot = viewDir.vector_dotMutiply(reflectDir) > 0 ? viewDir.vector_dotMutiply(reflectDir) : 0;
	// �߹�ķ����(Shininess)
	//һ������ķ����Խ�ߣ�����������Խǿ��ɢ���Խ�٣��߹��ͻ�ԽС
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

	//�������������������Ϣ�洢����Ҫ�о��Ĺ���ģ����ȥ
	//��face[i]�����Ϣ�洢��groundface[i]��

	// �洢���ABCD��Ϣ
	GetABC(face, groundFace.A, groundFace.B, groundFace.C, groundFace.D);

	// �洢��ĵ���Ϣ
	int pointsize = 3;
	for (int j = 0; j < pointsize; ++j)
	{
		//groundFace.points[j].polyID = i;	 //ÿһ�����������Ķ����
		groundFace.points[j].indexID = j; //��ȡ����������е������±�
		groundFace.points[j].point = face.point3D[j];
	}

	//�洢��ķ�������Ϣ
	groundFace.spmv.x() = groundFace.A;
	groundFace.spmv.y() = groundFace.B;
	groundFace.spmv.z() = groundFace.C;

	// ʹ�����з�����ָ���������ⲿ
	// ����һ����ͼ������ָ��ͼ����Ķ��������
	Vec4 center(0, 0, 0);
	Vec4 tempVec = center - groundFace.points[0].point;
	if (!SpMVdrector(groundFace.spmv, tempVec))
	{
		groundFace.spmv.x() *= -1;
		groundFace.spmv.y() *= -1;
		groundFace.spmv.z() *= -1;
	}

	//�õ�ÿ��������ÿһ����ķ�����
	//�������㴦�ķ�������λ��
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
	// 1��������
	Ia = Kar * Iar;
	// 2��������
	// ������
	Vec4 norm = tarv;
	//norm.vector_normalize();

	//�����Դ����������������
	//�����������x,y����ά�ռ�����
	double z = GetZdeep(groundFace[pixel].A, groundFace[pixel].B, groundFace[pixel].C, groundFace[pixel].D, x, y);
	// �����λ��
	Vec4 fragPos(x, y, z);

	// ���߷���
	// LightΪ��Դ����
	Vec4 lightDir = Light - fragPos;
	lightDir.vector_normalize();
	float diff = norm.vector_dotMutiply(lightDir) > 0 ? norm.vector_dotMutiply(lightDir) : 0;
	// ������Ӧ�û�Ҫ��һ���������ϵ��r*r������Ч����̫��
	/*double distance = sqrt(pow(norm.x - fragPos.x, 2) + pow(norm.y - fragPos.y, 2));
	double r = distance > 1 ? distance : 1;*/
	Id = diff * Kdr * Ipr;
	// 3�����淴��
	//vector3D viewPos(260, 220, -0.5);
	Vec4 viewPos = eye * 10;
	Vec4 viewDir = viewPos - fragPos;
	viewDir.vector_normalize();
	Vec4 reflectDir = getReflect(lightDir, norm);
	double dot = viewDir.vector_dotMutiply(reflectDir) > 0 ? viewDir.vector_dotMutiply(reflectDir) : 0;
	// �߹�ķ����(Shininess)
	//һ������ķ����Խ�ߣ�����������Խǿ��ɢ���Խ�٣��߹��ͻ�ԽС
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
	// 1��������
	Ia = Kar * Iar;
	// 2��������
	// ������
	Vec4 norm = tarv;
	norm.vector_normalize();
	// �����λ��
	Vec4 fragPos(x, y, z);
	fragPos = fragPos.vectorMultiplyMatrix3D(fragPos, m);
	//fragPos = fragPos * (1.0 / fragPos.h);

	// ���߷���
	// LightΪ��Դ����
	Vec4 lightDir = Light - fragPos;
	lightDir.vector_normalize();
	float diff = lightDir.vector_dotMutiply(norm) > 0 ? lightDir.vector_dotMutiply(norm) : 0;
	// ������Ӧ�û�Ҫ��һ���������ϵ��r*r������Ч����̫��
	/*double distance = sqrt(pow(norm.x - fragPos.x, 2) + pow(norm.y - fragPos.y, 2));
	double r = distance > 1 ? distance : 1;*/
	Id = diff * Kdr * Ipr;
	// 3�����淴��
	//vector3D viewPos(260, 220, -0.5);
	Vec4 viewPos = eye;
	Vec4 viewDir = viewPos - fragPos;
	viewDir.vector_normalize();
	Vec4 reflectDir = getReflect(lightDir*(-1), norm);
	double dot = reflectDir.vector_dotMutiply(viewDir) > 0 ? reflectDir.vector_dotMutiply(viewDir) : 0;
	// �߹�ķ����(Shininess)
	//һ������ķ����Խ�ߣ�����������Խǿ��ɢ���Խ�٣��߹��ͻ�ԽС
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











