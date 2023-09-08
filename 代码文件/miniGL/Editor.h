#pragma once
#include "Matrix3.h"

struct Geometry;
struct Dataset;
struct Layer;
class Editor
{
public:
	//�������ݼ�������ͼ��
	void apply(Dataset* pDataset, Matrix3& m);

	//����ͼ��
	void apply(Layer* pLayer, Matrix3& m);

	//����ͼ�ζ���
	void apply(Geometry* pGeometryDef, Matrix3& m);

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
};
