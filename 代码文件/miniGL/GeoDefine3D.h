#pragma once

#include "Vec3.h"

#include <vector>
#include "0_Matrix4.h"
#include "0_Vec4.h"

using namespace std;


///顶点属性启用类型
enum VertexAttributeType { vtXYZ = 1, vtCOLOR = 2, vtNORMAL = 4, vtUV = 8 };

///顶点属性
struct VertexAttribute
{
	Vec4 color;//颜色
	Vec3 normal;//法线
	double uv[2];//纹理坐标
};

///3D顶点
struct Vertex3D {
	Vertex3D(){}
	Vertex3D(Vec3& v) :pos(v) { }

	Vec4 pos;//顶点位置
	VertexAttribute attribute;//顶点属性

} ;

//三角形，不包括实际顶点数据，只记录顶点在顶点数据组中的索引号
struct Face
{
	Face()
	{
		set(0, 0, 0);
	}

	Face(int v0, int v1, int v2)
	{
		set(v0, v1, v2);
	}

	void set(int v0, int v1, int v2)
	{
		ids[0] = v0, ids[1] = v1, ids[2] = v2;
	}

	int operator[](int i) { return ids[i]; }//按下标获取三角形顶点在顶点数组中的索引

	int next(int i) { return ids[(i + 1) % 3]; }//下一个顶点id
	int ids[3];//保存顶点索引
};

///三角网
struct Mesh
{
	vector<Vertex3D> vertexSet;//顶点数组
	vector<Face> faceSet;//三角形数组
	unsigned vertexDeclaration;//顶点格式

	bool attributeEnabled(VertexAttributeType vt) { return vertexDeclaration & vt; }
	bool colorAttributeEnabled() { return attributeEnabled(vtCOLOR); }
	bool normalAttributeEnabled() { return attributeEnabled(vtNORMAL); }
	bool uvAttributeEnabled() { return attributeEnabled(vtUV); }
};

//3D包围盒
struct Box3D
{
	Box3D() { setBox( -1, -1, -1, 1, 1, 1 ); }
	Box3D(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax)
	{
		setBox(xmin, ymin, zmin, xmax, ymax, zmax);
	}

	void setBox(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax)
	{
		this->xmin = xmin, this->xmax = xmax, this->ymin = ymin, this->ymax = ymax, this->zmin = zmin, this->zmax = zmax;
	}

	Vec3 operator[](int index);

	Mesh* toMesh(int n);

	double xmin, ymin, zmin;
	double xmax, ymax, zmax;
};

///受限平面
struct Plane
{
	Plane()
	{
		setByCenter( 0, 0, 0, 2, 2);
	}

	void setByCenter(double x, double y, double z, double width, double height)
	{
		corners[0].set(x - width / 2, y - height / 2, z);
		corners[1].set(x + width / 2, y - height / 2, z);
		corners[2].set(x + width / 2, y + height / 2, z);
		corners[3].set(x - width / 2, y + height / 2, z);
	}

	Mesh* toMesh();

	Vec3 corners[4];
};

enum NodeType { ntGroup, ntTransform, ntModel };
struct GroupNode;

///结点基类，用于处理父子关系，本质是一棵多叉树
struct Node
{
	Node() { this->parent = NULL; }
	Node(GroupNode* parent);

	virtual ~Node() {}
	virtual NodeType getNodeType() = 0;

	GroupNode* parent;//父节点
};

///组结点，可以挂多个子节点
struct GroupNode : public Node
{
	GroupNode() :Node() {}
	GroupNode(GroupNode* parent) :Node(parent) {}

	virtual ~GroupNode() { for (int i = 0, size = children.size(); i < size; ++i) delete children[i]; }

	virtual NodeType getNodeType() { return ntGroup; }
	void addChild(Node* pChild) { children.push_back(pChild); }
	Node* operator[](int i) { return children[i]; }

	vector<Node*> children;//子节点集合
};

///变换结点，对子节点进行变换
struct TransformNode : public GroupNode
{
	TransformNode() :GroupNode() {}
	TransformNode(GroupNode* parent) :GroupNode(parent) {}

	virtual NodeType getNodeType() { return ntTransform; }

	Matrix4 transformMatrix;//变换矩阵
};

struct Material;
struct Renderable;
///模型结点，用于组织可渲染对象Renderable
struct ModelNode : public Node
{
	ModelNode() :Node() {}
	ModelNode(GroupNode* parent) :Node(parent) {}
	~ModelNode();

	virtual NodeType getNodeType() { return ntModel; }
	void addRenderable(Material* pMaterial, Mesh* pMesh);

	vector<Renderable*> renderableSet;
};

///可渲染对象,由几何属性mesh和材质属性material组成
struct Renderable
{
	Renderable(Material* material, Mesh* pMesh);
	~Renderable();

	Material* material;//材质
	Mesh* mesh;//三角网格
};

enum FillMode{	FILL_SOLID,	FILL_WIREFRAME,	FILL_VERTEX, FILL_2D, FILL_3D, FILL_NONE};
enum ShadingMode{	SHADING_FLAT,	SHADING_GOURAUD,	SHADING_PHONG };
enum CULLMode { CULL_NONE, CULL_CLOCKWISE, CULL_COUNTCLOCKWISE };//不剔除、前向面剔除、后向面剔除

///材质
struct Material
{
	Material();

	Vec4 ambient;//环境光
	Vec4 diffuse;//漫反射光
	Vec4 specular;//镜面反射光
	double shininess;//镜面反射系数高光指数

	//ShadingMode shadingMode;//光照模型
	//FillMode fillMode;//填充模式
};