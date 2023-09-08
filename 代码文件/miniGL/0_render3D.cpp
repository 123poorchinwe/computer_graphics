#include "0_render3D.h"
#include "Vec2.h"
#include"1_state.h"
#include"1_Rasterprint.h"
#include"Define.h"
#include"0_light.h"
//#include"1_LightModel.h"

//��������ͼ
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

//����ͼ��ʼģ�;���
void Renderer3D::setModelMatrix(Matrix4& matrix)
{
    g_modelMatrix = matrix;
}

//����ģ�ͽڵ�
void Renderer3D::buildScene(GroupNode* pRoot)
{
    {
        TransformNode* pTransformnode = new TransformNode(pRoot);
        pTransformnode->transformMatrix.makeScale(100, 100, 100);//���ڵ��ȷ�������ƽ��
        pTransformnode->transformMatrix *= Matrix4::translate(100, 0, 0);

        ModelNode* pmodelnode = new ModelNode(pTransformnode);
        pmodelnode->addRenderable(new Material(), Box3D().toMesh(8));
    }
    {
        TransformNode* pTransformnode = new TransformNode(pRoot);
        pTransformnode->transformMatrix.makeScale(50, 50, 50);//���ڵ��ȷ�������ƽ��
        pTransformnode->transformMatrix *= Matrix4::translate(-50, 0, 0);

        ModelNode* pmodelnode = new ModelNode(pTransformnode);
        pmodelnode->addRenderable(new Material(), Box3D().toMesh(8));
    }
    {
        TransformNode* pTransformnode = new TransformNode(pRoot);
        pTransformnode->transformMatrix.makeScale(200, 200, 200);//���ڵ��ȷ�������ƽ��
        pTransformnode->transformMatrix *= Matrix4::translate(0, 0, -100);
        pTransformnode->transformMatrix *= Matrix4::Rotate(DEG_RAD(90), 1);
        ModelNode* pmodelnode = new ModelNode(pTransformnode);
        pmodelnode->addRenderable(new Material(), Plane().toMesh());
    }
}

//�Բ��ʺ�������������Ⱦ
void Renderer3D::render(Renderable* pRenderable,Rendertype g_Rt)
{
    applyMaterial(pRenderable->material);
    render(pRenderable->mesh, g_Rt);
}

void Renderer3D::applyMaterial(Material* pMaterial)
{

}

int co = 0;//��¼�����ɫ
bool cull(Vertex3D pixelSet[3]);//��������


//��Ⱦpmesh
void Renderer3D::render(Mesh* pMesh,Rendertype g_Rt)
{
    int width = getWindowWidth(), height = getWindowHeight();
    if (width == 0 || height == 0) return;

    //�ܾ���=ģ�ͱ任*�۲�任*ͶӰ�任*�ӿڱ任
    //g_transformMatrix3D = g_modelMatrix * g_viewMatrix * g_projectMatrix * Matrix4::scale(0.5, 0.5, 1.0) * Matrix4::translate(0.5, 0.5, 0.0);
    g_transformMatrix3D = g_modelMatrix * g_viewMatrix * g_projectMatrix * g_viewport;

#pragma region vertex transform //������
    vector<Vertex3D>& vertexSet = pMesh->vertexSet;
    static vector<Vertex3D> transformedVertexSet;
    transformedVertexSet.resize(vertexSet.size());
    for (int i = 0, c = vertexSet.size(); i < c; ++i)
    {
        transformedVertexSet[i].attribute = vertexSet[i].attribute;
        transformedVertexSet[i].pos = vertexSet[i].pos * g_transformMatrix3D;
    }
#pragma endregion

    vector<Face>& faceSet = pMesh->faceSet;//��������������
    for (int i = 0, c = faceSet.size(); i < c; ++i)
    {
        Face& face = faceSet[i];
#pragma region cull
        int clipFlag[3] = { 0 };
        for (int j = 0; j < 3; ++j)
        {
            Vec4& pos = transformedVertexSet[face[j]].pos;//ʹ�òü���Ķ���
            double w = pos.w();
            double x = pos.x();
            double y = pos.y();
            double z = pos.z();
            if (w <= 0 || x<0 || x>w || y<0 || y>w || z<0 || z>w)
                clipFlag[j] = 1;


        }
        //���ò��ɼ��Ĳ��֣������µ�������
        if (clipFlag[0] || clipFlag[1] || clipFlag[2])//�򻯴�����һ�����㲻�ɼ����߳���������
            continue;
#pragma endregion
#pragma region viewportTransform

        ///TODO:ʹ�òü���������� 
        Vertex3D pixelSet[3];
        for (int j = 0; j < 3; ++j)
        {
            Vertex3D& v = transformedVertexSet[face[j]];//ʹ�òü���Ķ���
            Vec4& pos = v.pos;
            double r_w = 1.0 / pos.w();//͸�ӳ���
            //�ӿڱ任
            pixelSet[j].pos.x() = pos.x() * r_w * (width - 1);
            pixelSet[j].pos.y() = pos.y() * r_w * (height - 1);
            pixelSet[j].pos.z() = pos.z() * r_w;
            pixelSet[j].pos.w() = r_w;
            pixelSet[j].attribute = v.attribute;

        }

        //ʹ��z_buffer�����z���������������
        if (g_State.Fi11Mode == FILL_3D)
        {
            int color[4] = { BLUE,RED,YELLOW,GREEN };//��Ŀ����ɫ����һ������
            int id = co % 4;
            co = co + 1;
            g_Raster.polygonScan(pixelSet, 3, color[id]);//ÿ���渳�����ɫ
        }

#pragma endregion


#pragma region cull
        //�������޳�
        if (g_State.CullMode == CULL_COUNTCLOCKWISE)//�������޳�
        {
            if (g_State.CullMode != CULL_NONE && cull(pixelSet))
                continue;//������<=0��������������rasterize������
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
                //���ƿɼ�����������
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
/// �������޳�
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

