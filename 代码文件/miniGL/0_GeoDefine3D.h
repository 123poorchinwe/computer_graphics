#pragma once

#include <vector>
#include <algorithm>
#include "Graphic.h"
#include "Define.h"
#include <string>

using namespace std;

enum GeomType { gtUnkown = 0, gtPoint3D = 301, gtPolyline3D = 302, gtPolygon3D = 303, gtCircle3D = 304 , gtEllipse3D = 305 };

//3D包围盒
template<typename T>
struct _Box3D
{
	_Box3D() { valid = false; }
	_Box3D(T xmin, T ymin, T zmin, T xmax, T ymax, T zmax)
	{
		setBox(xmin, ymin, zmin ,xmax, ymax ,zmax);
	}

	T length() { return _xmax - _xmin; }//包围盒高度
	T width() { return _ymax - _ymin; }//包围盒宽度
	T height() { return _zmax - _zmin; }//包围盒高度
	T centerX() { return (_xmin + _xmax) / 2; }//包围盒中心点x坐标
	T centerY() { return (_ymin + _ymax) / 2; }//包围盒中心点y坐标
	T centerZ() { return (_zmin + _zmax) / 2; }//包围盒中心点z坐标
	T xmin() { return _xmin; }
	T ymin() { return _ymin; }
	T xmax() { return _xmax; }
	T ymax() { return _ymax; }
	T zmin() { return _zmin; }
	T zmax() { return _zmax; }

	void setBox(T xmin, T ymin, T zmin, T xmax, T ymax, T zmax)//设置包围盒范围
	{
		this->_xmin = xmin, this->_xmax = xmax, this->_ymin = ymin, this->_ymax = ymax, this->_zmin = zmin, this->_zmax = zmax;;
		valid = true;
	}

	void setBox(_Box3D& box)
	{
		setBox(box._xmin, box._ymin,box._zmin, box._xmax, box._ymax, box._zmax);
	}

	//使包围盒无效
	void invalidate()
	{
		valid = false;
	}

	//根据添加的点扩展包围盒
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

	//根据添加的包围盒扩展包围盒
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
	bool valid;//是否有效
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


//图形对象基类，供继承
struct Geometry3D
{
	virtual ~Geometry3D() {}
	virtual GeomType getGeomType() = 0; //获取图形对象类型
	virtual Box3D getEnvelop() = 0;//获取图形对象包围盒
	string label;
};


//点图像对象
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

//线图形对象
struct PolylineGeometry3D :Geometry3D
{
	virtual GeomType getGeomType() { return gtPolyline3D; }

	//运算符重载，获取第i个点
	Point3D& operator[](int i) { return pts[i]; }

	//添加点
	void addPoint(double x, double y, double z)
	{
		pts.push_back(Point3D(x, y, z));
		envelop.expand(x, y, z);
	}

	/// <summary>
	/// 刷新包围盒
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
	//所有点
	vector<Point3D> pts;
	Box3D envelop;
};

//多边形图形对象
struct PolygonGeometry3D :PolylineGeometry3D
{
	virtual GeomType getGeomType() { return gtPolygon3D; }
};

//球对象

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

//椭球对象

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

//图层
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

	//析构函数删除所有图形对象
	virtual ~Layer3D()
	{
		for (size_t i = 0, size = geometrySet.size(); i < size; ++i) delete geometrySet[i];
	}

	//运算符重载，返回第i个图形对象
	Geometry3D* operator[](int i) { return geometrySet[i]; }

	//添加图像对象
	void addGeometry(Geometry3D* pGeometry, bool updateEnvelop = false)
	{
		geometrySet.push_back(pGeometry);
		if (updateEnvelop)
			envelop.expand((Box3D&)pGeometry->getEnvelop());
	}

	//设置图层范围
	void setEnvelop(double xmin, double ymin, double zmin,double xmax, double ymax, double zmax)
	{
		envelop.setBox(xmin, ymin,zmin, xmax, ymax, zmax);
	}

	//获取图层包含的图形对象数量
	int getGeometryCount() { return geometrySet.size(); }

	vector<Geometry3D*> geometrySet;//图形对象集合
	Box3D envelop;//图层范围对应的包围盒
	GeomType geomType;//图层类型
	Color layerColor = BLUE;//图层颜色
};

//数据集
struct Dataset3D
{
	//析构函数删除图层
	virtual ~Dataset3D()
	{
		for (size_t i = 0, size = layerSet.size(); i < size; ++i) delete layerSet[i];
	}

	//运算符重载，返回第i个图层
	Layer3D* operator[](int i) { return layerSet[i]; }

	//遍历整个数据集，选出layer中存在的图形
	int indexOf(Layer3D* pLayer) {
		for (int i = 0; i < layerSet.size(); ++i)
		{
			if (pLayer = layerSet[i]) return i;
		}
		return -1;
	}

	//获取图层数
	int getLayerCount() { return layerSet.size(); }

	//添加图层
	void addLayer(Layer3D* pLayer)
	{
		layerSet.push_back(pLayer);
	}

	//图层集合
	vector<Layer3D*> layerSet;
};
