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
	//static void Zbuffer(Vec4 pts0, Vec4 pts1, Vec4 pts2);//深度缓存(调用了光照)
	//void Zbuffer(Vertex3D pixelSet[]);
	//Vec3 calculateLighting(Vec3& Normal, Vec3& FragPos);//phong算法计算光照
	////Renderer3D() ;
	Matrix4 g_modelMatrix;//模型变换矩阵
	Matrix4 g_viewMatrix;//视图变换矩阵
	Matrix4 g_projectMatrix;//投影变换矩阵
	Matrix4 g_viewport;//视口变换矩阵
	Matrix4 g_transformMatrix3D;//总
	enum Rendertype
	{
		otwireframe, otfiller, otperspective, otorthogonal
	};//变换形式定义，方便切换

	void rendererSceneNode(Node* pNide, Matrix4& parentTransform, Rendertype g_Rt);//场景图
	void setModelMatrix(Matrix4& matrix);//设置初始模型矩阵
	void buildScene(GroupNode* pRoot);	//场景模型节点

	
	void render(Renderable* pRenderable, Rendertype g_Rt);//对material和pMesh渲染
	void render(Mesh* pMesh, Rendertype g_Rt);//渲染pMesh

	
protected:	

	void applyMaterial(Material* pMaterial);

};

