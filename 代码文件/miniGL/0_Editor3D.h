#pragma once
#include "0_Matrix4.h"
//#include "Matrix3.h"

//#��ʵ��������û�������ô���д�3D��2D��չʾ:���ص���x��y��

struct Geometry3D;
struct Dataset3D;
struct Layer3D;
class Editor3D
{
public:
	//�������ݼ�������ͼ��
	void apply(Dataset3D* pDataset, Matrix4& m);

	//����ͼ��
	void apply(Layer3D* pLayer, Matrix4& m);

	//����ͼ�ζ���
	void apply(Geometry3D* pGeometryDef, Matrix4& m);

	//ƽ�ƣ�������Ϊ��λ
	void translate(double dx, double dy, double dz);

	//��ת����ʱ��Ϊ���� �Ի���Ϊ��λ
	void rotate(double rad);

	//������ת����ʱ��Ϊ���� �Ի���Ϊ��λ
	void rotate(double x, double y, double z,double rad);

	//���ķ���
	void scale(double s);

	//�������
	void scale(double x, double y, double z,double s);
};
