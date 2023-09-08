#pragma once

#include "Graphic.h"
#include <vector>
#include "GeoDefine3D.h"

struct Geometry;
struct Dataset;
struct Layer;

class Renderer3D
{
public:
	//static void Zbuffer(Vec4 pts0, Vec4 pts1, Vec4 pts2);//��Ȼ���(�����˹���)
	//void Zbuffer(Vertex3D pixelSet[]);
	//Vec3 calculateLighting(Vec3& Normal, Vec3& FragPos);//phong�㷨�������
	////Renderer3D() ;
	Matrix4 g_modelMatrix;//ģ�ͱ任����
	Matrix4 g_viewMatrix;//��ͼ�任����
	Matrix4 g_projectMatrix;//ͶӰ�任����
	Matrix4 g_viewport;//�ӿڱ任����
	Matrix4 g_transformMatrix3D;//��
	enum Rendertype
	{
		otwireframe, otfiller, otperspective, otorthogonal
	};//�任��ʽ���壬�����л�

	void rendererSceneNode(Node* pNide, Matrix4& parentTransform, Rendertype g_Rt);//����ͼ
	void setModelMatrix(Matrix4& matrix);//���ó�ʼģ�;���
	void buildScene(GroupNode* pRoot);	//����ģ�ͽڵ�

	
	void render(Renderable* pRenderable, Rendertype g_Rt);//��material��pMesh��Ⱦ
	void render(Mesh* pMesh, Rendertype g_Rt);//��ȾpMesh

	
protected:	

	void applyMaterial(Material* pMaterial);

};

