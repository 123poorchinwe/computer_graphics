#pragma once

#include "Matrix3.h"
#include "Graphic.h"
#include <vector>

struct Geometry;
struct Dataset;
struct Layer;
class Selector;
class Renderer2D
{
public:

	Renderer2D() { mSelectColor = RED; };

	//ͼ���ʼ��
	void init(double dx, double dy, double w, double h, int WWidth, int WHeight);

	//��ԭ��ͼ
	virtual void reset();

	//���ô��ڣ�2D������ͶӰ
	virtual void setWindow(double left, double right, double bottom, double top);

	//�ӿڴ�С�ı�
	virtual void setViewport(int x, int y, int width, int height);

	//�������ݼ�������ͼ��
	virtual void Renderer2D::render(Dataset* pDataset);

	//����ͼ��
	virtual void Renderer2D::render(Layer* pLayer);

	//����ͼ�ζ���
	virtual void Renderer2D::render(Geometry* pGeometryDef);

	void applyViewMatrix(Matrix3& m);

	//ƽ�ƣ�������Ϊ��λ
	void translate(double dx, double dy);

	//��ת����ʱ��Ϊ���� �Ի���Ϊ��λ
	void rotate(double rad);

	//������ת����ʱ��Ϊ���� �Ի���Ϊ��λ
	void rotate(double x, double y, double rad);

	//���ķ���
	void scale(double s);

	//�������
	void scale(double x, double y, double s);


	//���񱳾�
	void renderBackground();

	Matrix3 mTransformMatrix;//�任���󣬼�¼�����ӿڡ�ģ�ͱ任����	
protected:	

	Color mSelectColor;//ѡ����Ⱦ��ɫ
	Matrix3 m;//�任����	
	double mOrigXCenter_map, mOrigYCenter_map, mOrigWidth_map, mOrigHeight_map;//��Ӧ����ԭ��ĵ�ͼ��
	double mOrigScale;
	double mX_win, mY_win;
	double mWidth_win, mHeight_win;
};

