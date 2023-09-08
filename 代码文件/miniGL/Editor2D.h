#pragma once
#include <vector>
#include "Matrix3.h"

class Renderer2D;
struct Geometry;
class Editor2D
{
public:
	Editor2D(Renderer2D* pRenderer2D) {
		mpRenderer2D = pRenderer2D;
	}

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

	void updateGeometrySet(std::vector<Geometry*>& geometrySet );

	void updateGeometry(Geometry* pGeometry) {
		updateGeometry(pGeometry, m);
	}

	static void updateGeometry(Geometry* pGeometry , Matrix3& m );

	Renderer2D* mpRenderer2D;
	Matrix3 m;
};

