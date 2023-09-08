#include "0_render3D.h"
#include "Vec2.h"
#include"1_state.h"
#include"1_Rasterprint.h"
#include"Define.h"
#include"0_light.h"
//#include"1_LightModel.h"

//构建场景图
void Renderer3D::rendererSceneNode(Node* pNode, Matrix4& parentTransform, Rendertype g_Rt)
{
    NodeType nt = pNode->getNodeType();
    switch (nt)
    {

    case ntTransform:
    case ntGroup:
    {
        GroupNode* pParentNode = (GroupNode*)pNode;
        for (int i = 0, size = pParentNode->children.size(); i < size; ++i)
        {
            if (nt == ntGroup)
                rendererSceneNode(pParentNode->children[i], parentTransform, g_Rt);
            else
                rendererSceneNode(pParentNode->children[i], ((TransformNode*)pNode)->transformMatrix * parentTransform, g_Rt);
        }
    }
    break;
    case ntModel:
    {
        setModelMatrix(parentTransform);
        ModelNode* pModelNode = (ModelNode*)pNode;
        for (int i = 0, size = pModelNode->renderableSet.size(); i < size; ++i)
        {
            render(pModelNode->renderableSet[i],g_Rt);
        }
    }
    break;
    default:
        break;
    }
}

//场景图初始模型矩阵
void Renderer3D::setModelMatrix(Matrix4& matrix)
{
    g_modelMatrix = matrix;
}

//场景模型节点
void Renderer3D::buildScene(GroupNode* pRoot)
{
    {
        TransformNode* pTransformnode = new TransformNode(pRoot);
        pTransformnode->transformMatrix.makeScale(100, 100, 100);//根节点先放缩，再平移
        pTransformnode->transformMatrix *= Matrix4::translate(100, 0, 0);

        ModelNode* pmodelnode = new ModelNode(pTransformnode);
        pmodelnode->addRenderable(new Material(), Box3D().toMesh(8));
    }
    {
        TransformNode* pTransformnode = new TransformNode(pRoot);
        pTransformnode->transformMatrix.makeScale(50, 50, 50);//根节点先放缩，再平移
        pTransformnode->transformMatrix *= Matrix4::translate(-50, 0, 0);

        ModelNode* pmodelnode = new ModelNode(pTransformnode);
        pmodelnode->addRenderable(new Material(), Box3D().toMesh(8));
    }
    {
        TransformNode* pTransformnode = new TransformNode(pRoot);
        pTransformnode->transformMatrix.makeScale(200, 200, 200);//根节点先放缩，再平移
        pTransformnode->transformMatrix *= Matrix4::translate(0, 0, -100);
        pTransformnode->transformMatrix *= Matrix4::Rotate(DEG_RAD(90), 1);
        ModelNode* pmodelnode = new ModelNode(pTransformnode);
        pmodelnode->addRenderable(new Material(), Plane().toMesh());
    }
}

//对材质和三角网进行渲染
void Renderer3D::render(Renderable* pRenderable,Rendertype g_Rt)
{
    applyMaterial(pRenderable->material);
    render(pRenderable->mesh, g_Rt);
}

void Renderer3D::applyMaterial(Material* pMaterial)
{

}

int co = 0;//记录随机颜色
bool cull(Vertex3D pixelSet[3]);//函数声明


//渲染pmesh
void Renderer3D::render(Mesh* pMesh,Rendertype g_Rt)
{
    int width = getWindowWidth(), height = getWindowHeight();
    if (width == 0 || height == 0) return;

    //总矩阵=模型变换*观察变换*投影变换*视口变换
    //g_transformMatrix3D = g_modelMatrix * g_viewMatrix * g_projectMatrix * Matrix4::scale(0.5, 0.5, 1.0) * Matrix4::translate(0.5, 0.5, 0.0);
    g_transformMatrix3D = g_modelMatrix * g_viewMatrix * g_projectMatrix * g_viewport;

#pragma region vertex transform //处理顶点
    vector<Vertex3D>& vertexSet = pMesh->vertexSet;
    static vector<Vertex3D> transformedVertexSet;
    transformedVertexSet.resize(vertexSet.size());
    for (int i = 0, c = vertexSet.size(); i < c; ++i)
    {
        transformedVertexSet[i].attribute = vertexSet[i].attribute;
        transformedVertexSet[i].pos = vertexSet[i].pos * g_transformMatrix3D;
    }
#pragma endregion

    vector<Face>& faceSet = pMesh->faceSet;//把三角网画出来
    for (int i = 0, c = faceSet.size(); i < c; ++i)
    {
        Face& face = faceSet[i];
#pragma region cull
        int clipFlag[3] = { 0 };
        for (int j = 0; j < 3; ++j)
        {
            Vec4& pos = transformedVertexSet[face[j]].pos;//使用裁剪后的顶点
            double w = pos.w();
            double x = pos.x();
            double y = pos.y();
            double z = pos.z();
            if (w <= 0 || x<0 || x>w || y<0 || y>w || z<0 || z>w)
                clipFlag[j] = 1;


        }
        //剪裁不可见的部分，生成新的三角形
        if (clipFlag[0] || clipFlag[1] || clipFlag[2])//简化处理：有一个顶点不可见，踢出整个三角
            continue;
#pragma endregion
#pragma region viewportTransform

        ///TODO:使用裁剪后的三角形 
        Vertex3D pixelSet[3];
        for (int j = 0; j < 3; ++j)
        {
            Vertex3D& v = transformedVertexSet[face[j]];//使用裁剪后的顶点
            Vec4& pos = v.pos;
            double r_w = 1.0 / pos.w();//透视除法
            //视口变换
            pixelSet[j].pos.x() = pos.x() * r_w * (width - 1);
            pixelSet[j].pos.y() = pos.y() * r_w * (height - 1);
            pixelSet[j].pos.z() = pos.z() * r_w;
            pixelSet[j].pos.w() = r_w;
            pixelSet[j].attribute = v.attribute;

        }

        //使用z_buffer对深度z进行三角网面填充
        if (g_State.Fi11Mode == FILL_3D)
        {
            int color[4] = { BLUE,RED,YELLOW,GREEN };//将目标颜色放入一个数组
            int id = co % 4;
            co = co + 1;
            g_Raster.polygonScan(pixelSet, 3, color[id]);//每个面赋予随机色
        }

#pragma endregion


#pragma region cull
        //后向面剔除
        if (g_State.CullMode == CULL_COUNTCLOCKWISE)//后向面剔除
        {
            if (g_State.CullMode != CULL_NONE && cull(pixelSet))
                continue;//满足差积<=0的三角网不进入rasterize，不画
        }
        else if (g_State.CullMode == CULL_CLOCKWISE)
        {
            if (g_State.CullMode != CULL_NONE && !cull(pixelSet))
                continue;
        }

#pragma endregion

#pragma region rasterize

        switch (g_Rt)
        {
        case otwireframe:
            for (int j = 0; j < 3; ++j)
            {
                //绘制可见的像素坐标
                Vec4& pt1 = pixelSet[j].pos;
                Vec4& pt2 = pixelSet[(j + 1) % 3].pos;
                //drawLine(pt1.x(), pt1.y(), pt2.x(), pt2.y());
                g_Raster.DDA_Line(pt1.x(), pt1.y(), pt2.x(), pt2.y(), BLACK);

            }
            break;
        case otfiller:
            Raster1::Zbuffer(pixelSet[0].pos, pixelSet[1].pos, pixelSet[2].pos);
            break;
        }

    }
#pragma endregion
}
/// <summary>
/// 后向面剔除
/// </summary>
/// <param name="pixelSet"></param>
/// <returns></returns>
bool cull(Vertex3D pixelSet[3])
{
    Vec2 p1, p2;
    p1.x() = pixelSet[1].pos.x() - pixelSet[0].pos.x();
    p1.y() = pixelSet[1].pos.y() - pixelSet[0].pos.y();

    p2.x() = pixelSet[2].pos.x() - pixelSet[0].pos.x();
    p2.y() = pixelSet[2].pos.y() - pixelSet[0].pos.y();
    if ((p1 ^ p2) <= 0)
    {
        return true;
    }
    else
        return false;
}

