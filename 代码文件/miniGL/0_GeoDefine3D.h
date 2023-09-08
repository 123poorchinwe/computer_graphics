#pragma once

#include <vector>
#include <algorithm>
#include "Graphic.h"
#include "Define.h"
#include <string>

using namespace std;

enum GeomType { gtUnkown = 0, gtPoint3D = 301, gtPolyline3D = 302, gtPolygon3D = 303, gtCircle3D = 304 , gtEllipse3D = 305 };

//3D��Χ��
template<typename T>
struct _Box3D
{
	_Box3D() { valid = false; }
	_Box3D(T xmin, T ymin, T zmin, T xmax, T ymax, T zmax)
	{
		setBox(xmin, ymin, zmin ,xmax, ymax ,zmax);
	}

	T length() { return _xmax - _xmin; }//��Χ�и߶�
	T width() { return _ymax - _ymin; }//��Χ�п��
	T height() { return _zmax - _zmin; }//��Χ�и߶�
	T centerX() { return (_xmin + _xmax) / 2; }//��Χ�����ĵ�x����
	T centerY() { return (_ymin + _ymax) / 2; }//��Χ�����ĵ�y����
	T centerZ() { return (_zmin + _zmax) / 2; }//��Χ�����ĵ�z����
	T xmin() { return _xmin; }
	T ymin() { return _ymin; }
	T xmax() { return _xmax; }
	T ymax() { return _ymax; }
	T zmin() { return _zmin; }
	T zmax() { return _zmax; }

	void setBox(T xmin, T ymin, T zmin, T xmax, T ymax, T zmax)//���ð�Χ�з�Χ
	{
		this->_xmin = xmin, this->_xmax = xmax, this->_ymin = ymin, this->_ymax = ymax, this->_zmin = zmin, this->_zmax = zmax;;
		valid = true;
	}

	void setBox(_Box3D& box)
	{
		setBox(box._xmin, box._ymin,box._zmin, box._xmax, box._ymax, box._zmax);
	}

	//ʹ��Χ����Ч
	void invalidate()
	{
		valid = false;
	}

	//������ӵĵ���չ��Χ��
	void expand(T x, T y,T z)
	{
		if (!valid) {
			_xmin = _xmax = x;
			_ymin = _ymax = y;
			_zmin = _zmax = y;
			valid = true;
		}
		else
		{
			_xmin = min(_xmin, x);
			_xmax = max(_xmax, x);
			_ymin = min(_ymin, y);
			_ymax = max(_ymax, y);
			_zmin = min(_zmin, y);
			_zmax = max(_zmax, y);
		}
	}

	//������ӵİ�Χ����չ��Χ��
	void expand(_Box3D<T>& box) {
		if (box.valid)
		{
			if (!valid)
				setBox(box);
			else
			{
				expand(box.xmin(), box.ymin() , box.zmin());
				expand(box.xmax(), box.ymax() , box.zmax());
				
			}
		}
	}

protected:
	T _xmin, _ymin, _zmin;
	T _xmax, _ymax, _zmax;
	bool valid;//�Ƿ���Ч
};

typedef _Box3D<double> Box3D;


//Point3D
template<typename T>
struct _Point3D
{
	_Point3D() { x = y = z = 0; }
	_Point3D(T x, T y, T z) { this->x = x, this->y = y, this->z = z; }

	T x, y ,z;
};

typedef _Point3D<double> Point3D;


//ͼ�ζ�����࣬���̳�
struct Geometry3D
{
	virtual ~Geometry3D() {}
	virtual GeomType getGeomType() = 0; //��ȡͼ�ζ�������
	virtual Box3D getEnvelop() = 0;//��ȡͼ�ζ����Χ��
	string label;
};


//��ͼ�����
struct PointGeometry3D :Geometry3D
{
	double x, y, z;

	PointGeometry3D()
	{
		x = y = z = 0;
	}
	PointGeometry3D(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	virtual GeomType getGeomType() { return gtPoint3D; }
	virtual Box3D getEnvelop() { return Box3D(x, y, z, x, y, z); }

};

//��ͼ�ζ���
struct PolylineGeometry3D :Geometry3D
{
	virtual GeomType getGeomType() { return gtPolyline3D; }

	//��������أ���ȡ��i����
	Point3D& operator[](int i) { return pts[i]; }

	//��ӵ�
	void addPoint(double x, double y, double z)
	{
		pts.push_back(Point3D(x, y, z));
		envelop.expand(x, y, z);
	}

	/// <summary>
	/// ˢ�°�Χ��
	/// </summary>
	void refreshEnvelop()
	{
		envelop.invalidate();
		for (int i = 0; i < pts.size(); ++i)
		{
			envelop.expand(pts[i].x, pts[i].y, pts[i].z);
		}
	}

	virtual Box3D getEnvelop() { return envelop; }
	/*const vector<Point2D>& getPts() const
	{
		return pts;
	}*/
	vector<Point3D>& getPts()
	{
		return pts;
	}
protected:
	//���е�
	vector<Point3D> pts;
	Box3D envelop;
};

//�����ͼ�ζ���
struct PolygonGeometry3D :PolylineGeometry3D
{
	virtual GeomType getGeomType() { return gtPolygon3D; }
};

//�����

struct CircleGeometry3D :Geometry3D
{
	CircleGeometry3D()
	{
		x = y = z = 0;
		r = 0;
	}
	CircleGeometry3D(double x, double y, double z, double r)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = r;
	}

	virtual GeomType getGeomType() { return gtCircle3D; }
	virtual Box3D getEnvelop() { return Box3D(x - r, y - r,z - r, x + r, y + r, z + r); }

	double x, y, z;
	double r;
};

//�������

//struct Ellipsegeometry3D :Geometry3D
//{
//	Ellipsegeometry3D()
//	{
//		x = y = z = 0;
//	}
//	Ellipsegeometry3D(double x, double y, double z)
//	{
//		this->x = x;
//		this->y = y;
//		this->z = z;
//		
//	}
//
//	virtual GeomType getgeomtype() { return gtEllipse3D; }
//	virtual Box3D getenvelop() { return Box3D(x, y, z, x,y,z); }
//
//	//void getcenter(double& x, double& y ,double &z) { x = (x1 + x2) * 0.5, y = (y1 + y2) * 0.5; }
//	/*double getwidth() { return x - x1; }
//	double getheight() { return y2 - y1; }*/
//
//	double x, y,z;
//	//double x2, y2;
//};


//struct Symbol;
//struct SymbolGeometry: public Geometry
//{
//	SymbolGeometry( Geometry* pGeometry, Symbol* pSymbol )
//	{ 
//		this->pGeometry = pGeometry;
//		this->pSymbol = pSymbol;
//	}
//	virtual GeomType getGeomType(){ return pGeometry->getGeomType(); }
//
//	Geometry* pGeometry ;
//	Symbol* pSymbol;
//};

//ͼ��
struct Layer3D
{
	Layer3D()
	{
		geomType = gtUnkown;
	}

	Layer3D(GeomType geomType)
	{
		this->geomType = geomType;
	}

	//��������ɾ������ͼ�ζ���
	virtual ~Layer3D()
	{
		for (size_t i = 0, size = geometrySet.size(); i < size; ++i) delete geometrySet[i];
	}

	//��������أ����ص�i��ͼ�ζ���
	Geometry3D* operator[](int i) { return geometrySet[i]; }

	//���ͼ�����
	void addGeometry(Geometry3D* pGeometry, bool updateEnvelop = false)
	{
		geometrySet.push_back(pGeometry);
		if (updateEnvelop)
			envelop.expand((Box3D&)pGeometry->getEnvelop());
	}

	//����ͼ�㷶Χ
	void setEnvelop(double xmin, double ymin, double zmin,double xmax, double ymax, double zmax)
	{
		envelop.setBox(xmin, ymin,zmin, xmax, ymax, zmax);
	}

	//��ȡͼ�������ͼ�ζ�������
	int getGeometryCount() { return geometrySet.size(); }

	vector<Geometry3D*> geometrySet;//ͼ�ζ��󼯺�
	Box3D envelop;//ͼ�㷶Χ��Ӧ�İ�Χ��
	GeomType geomType;//ͼ������
	Color layerColor = BLUE;//ͼ����ɫ
};

//���ݼ�
struct Dataset3D
{
	//��������ɾ��ͼ��
	virtual ~Dataset3D()
	{
		for (size_t i = 0, size = layerSet.size(); i < size; ++i) delete layerSet[i];
	}

	//��������أ����ص�i��ͼ��
	Layer3D* operator[](int i) { return layerSet[i]; }

	//�����������ݼ���ѡ��layer�д��ڵ�ͼ��
	int indexOf(Layer3D* pLayer) {
		for (int i = 0; i < layerSet.size(); ++i)
		{
			if (pLayer = layerSet[i]) return i;
		}
		return -1;
	}

	//��ȡͼ����
	int getLayerCount() { return layerSet.size(); }

	//���ͼ��
	void addLayer(Layer3D* pLayer)
	{
		layerSet.push_back(pLayer);
	}

	//ͼ�㼯��
	vector<Layer3D*> layerSet;
};
