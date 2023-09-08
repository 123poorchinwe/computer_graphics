#include "GeoDefine3D.h"

Vec3 Box3D::operator[](int index)
{
	switch (index)
	{
	case 0:
		return Vec3(xmin, ymin, zmin);
		break;
	case 1:
		return Vec3(xmax, ymin, zmin);
		break;
	case 2:
		return Vec3(xmax, ymax, zmin);
		break;
	case 3:
		return Vec3(xmin, ymax, zmin);
		break;
	case 4:
		return Vec3(xmin, ymin, zmax);
		break;
	case 5:
		return Vec3(xmax, ymin, zmax);
		break;
	case 6:
		return Vec3(xmax, ymax, zmax);
		break;
	case 7:
		return Vec3(xmin, ymax, zmax);
		break;
	}
	throw;
}

//三维盒子中的三角网绘制
Mesh* Box3D::toMesh(int n)
{
	Mesh* pMesh = new Mesh();
	pMesh->vertexSet.reserve(n);
	for (int i = 0; i < n; ++i)
	{
		pMesh->vertexSet.push_back(Vertex3D((*this)[i]));
	}

	pMesh->faceSet.reserve(12);

	//left
	pMesh->faceSet.push_back(Face(7, 3, 0));
	pMesh->faceSet.push_back(Face(0, 4, 7));

	//back
	pMesh->faceSet.push_back(Face(0, 3, 1));
	pMesh->faceSet.push_back(Face(1, 3, 2));

	//right
	pMesh->faceSet.push_back(Face(1, 2, 5));
	pMesh->faceSet.push_back(Face(2, 6, 5));


	int nextBase = 0;

	//top
	pMesh->faceSet.push_back(Face(nextBase + 2, nextBase + 3, nextBase + 6));
	pMesh->faceSet.push_back(Face(nextBase + 3, nextBase + 7, nextBase + 6));

	//front
	pMesh->faceSet.push_back(Face(nextBase + 4, nextBase + 5, nextBase + 7));
	pMesh->faceSet.push_back(Face(nextBase + 5, nextBase + 6, nextBase + 7));

	//bottom
	pMesh->faceSet.push_back(Face(nextBase + 0, nextBase + 1, nextBase + 4));
	pMesh->faceSet.push_back(Face(nextBase + 1, nextBase + 5, nextBase + 4));

	return pMesh;
}

//三维视图中的平面
Mesh* Plane::toMesh()
{
	Mesh* pMesh = new Mesh();
	pMesh->vertexSet.reserve(4);
	for (int i = 0; i < 4; ++i)
	{
		pMesh->vertexSet.push_back(Vertex3D(corners[i]));
	}

	pMesh->faceSet.reserve(2);

	pMesh->faceSet.push_back(Face(0, 1, 3));
	pMesh->faceSet.push_back(Face(1, 2, 3));
	return pMesh;
}

//节点
Node::Node(GroupNode* parent)
{
	this->parent = parent;
	parent->addChild(this);
}

ModelNode::~ModelNode()
{
	for (int i = 0; i < renderableSet.size(); ++i) delete renderableSet[i];
}

void ModelNode::addRenderable(Material* pMaterial, Mesh* pMesh)
{
	renderableSet.push_back(new Renderable(pMaterial, pMesh));
}
//渲染
Renderable::Renderable(Material* material, Mesh* mesh)
{
	this->material = material;
	this->mesh = mesh;
}

Renderable::~Renderable()
{
	delete mesh;
	delete material;
}

//相关的光照定义
Material::Material()
{
	ambient.set(0.2, 0.2, 0.2, 1.0);
	diffuse.set(0.8, 0.8, 0.8, 1.0);
	specular.set(0.8, 0.8, 0.8, 1.0);
	shininess = 25.0;

}

