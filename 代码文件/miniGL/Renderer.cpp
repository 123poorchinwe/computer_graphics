#define _CRT_SECURE_NO_WARNINGS 1
#include "Renderer2D.h"
#include "GeoDefine.h"
#include "Selector.h"
#include "Graphic.h"
#include "Vec3.h"
#include"1_Rasterprint.h"
#include"1_state.h"
#include"1_Grid.h"

void Renderer2D::init(double dx, double dy, double w, double h, int WWidth, int WHeight) {
	/*添加shp图层的时候用到*/
	double s = std::min(WWidth / w, WHeight / h);

	w = WWidth / s;
	h = WHeight / s;
	/*对shp进行平移和放缩()，使之能够在屏幕的中央显示*/
	m.makeTranslate(w / 2 - dx, h / 2 - dy);
	m *= Matrix3::scale(s, s);
	mTransformMatrix *= m;
}



//重置
void Renderer2D::reset()
{
	setWindow(mOrigXCenter_map - mOrigWidth_map * 0.5, mOrigXCenter_map + mOrigWidth_map * 0.5,
		mOrigYCenter_map - mOrigHeight_map * 0.5, mOrigYCenter_map + mOrigHeight_map * 0.5);

	double w = getWindowWidth();
	double h = getWindowHeight();

	setViewport(mX_win * w, mY_win * h, mWidth_win * w, mHeight_win * h);
}
//视口大小改变
void Renderer2D::setViewport(int x, int y, int width, int height)
{
	x = mOrigXCenter_map;
	y = mOrigYCenter_map;
	width = getWindowWidth();
	height = getWindowHeight();
}

void Renderer2D::setWindow(double left, double right, double bottom, double top)
{
	left = 0;
	right = getWindowWidth();
	bottom = getWindowHeight();
	top = 0;
}
//渲染数据集
void Renderer2D::render(Dataset* pDataset)
{
	for (int i = pDataset->getLayerCount() - 1; i >= 0; --i)
	{
		Layer* pLayer = (*pDataset)[i];
		render(pLayer);
	}
}
//渲染图层
void Renderer2D::render(Layer* pLayer)
{
	setPenColor(pLayer->layerColor);

	for (int i = 0, size = pLayer->getGeometryCount(); i < size; ++i)
	{
		render((*pLayer)[i]);
	}
}

//对每个图形进行渲染
void Renderer2D::render(Geometry* pGeometryDef)
{
		
	switch (pGeometryDef->getGeomType())
	{
	case gtPoint:
	{
		PointGeometry* pGeometry = (PointGeometry*)pGeometryDef;
		
		Vec3 v(pGeometry->x, pGeometry->y);
		v *= m;
		setPixel(v.x(), v.y(), pGeometry->mycolor);//绘制
	}
	break;
	case gtPolyline:
	{
		PolylineGeometry* pGeometry = (PolylineGeometry*)pGeometryDef;//转化为实际图形类型
		
		const vector<Point2D>& pts = pGeometry->getPts();//获得图形顶点集合
		for (int i = 0, ptsCount = pts.size(); i < ptsCount - 1; ++i)
		{
			Vec3 v0(pts[i].x, pts[i].y);
			Vec3 v1(pts[i + 1].x, pts[i + 1].y);
			v0 *= m;
			v1 *= m;

			 if(g_State.DrawLineMethod==dlDDA)
			//drawLine(v0.x(), v0.y(), v1.x(), v1.y());
			    g_Raster.DDA_Line(v0.x(), v0.y(), v1.x(), v1.y(), pGeometry->mycolor);
			else if(g_State.DrawLineMethod == dlMidBresenham)
			    g_Raster.bresenham(v0.x(), v0.y(), v1.x(), v1.y(), getPenColor());
			 //网格绘制
			 if (g_State.DrawMode != dmPixel)
			 {
				 int xx= v0.x(), yy= v0.y();
				 g_Grid.pixelToGrid(xx,yy,xx,yy);				 
				 g_Grid.drawCell(xx, yy, BLACK);

			 }
		}
	}
	break;
	case gtPolygon:
	{
		PolygonGeometry* pGeometry = (PolygonGeometry*)pGeometryDef;//转化为实际图形类型
		const vector<Point2D>& pts = pGeometry->getPts();//获得图形顶点集合
		size_t ptsCount = pts.size();
		vector <PixelPoint> _pts(ptsCount);//drawPolygon函数参数是PixelPoint数组，_pts用于接收Point2D转化的PixelPoint数组
		for (int i = 0; i < ptsCount; ++i)
		{
			Vec3 v(pts[i].x, pts[i].y);
			v *= m;
			_pts[i].x = v.x();
			_pts[i].y = v.y();
		}
		//drawPolygon(_pts.data(), ptsCount);
		
		//多边形线框
		g_Raster.drawPolygonline(_pts.data(), ptsCount, pGeometry->mycolor);
		//填充
		if (g_State.Fi11Mode == FILL_2D)
		{
			if(g_State.DrawPolygonMethod==dpXScanline)			
			    //g_Raster.polygonScan(_pts.data(), ptsCount, p1);
			    g_Raster.drawPolygon(_pts.data(), ptsCount, pGeometry->mycolor);
			else if (g_State.DrawPolygonMethod == dpBoundFill)
			{
				g_Raster.BoundaryFill((_pts[0].x+_pts[ptsCount/2].x)/2, (_pts[0].y + _pts[ptsCount/2].y) / 2, getPenColor(), BLACK);
				/*for (int i = 0; i < ptsCount; ++i)
				{
					g_Raster.BoundaryFill(_pts[i].x, _pts[i].y, BLUE, YELLOW);
				}*/
	
			}
		}		   
	}
	break;
	case gtCircle:
	{
		CircleGeometry* pGeometry = (CircleGeometry*)pGeometryDef;//转化为实际圆形
		
		Point2D center = pGeometry->GetCenter();
		double r = pGeometry->r;
		//if (Matrix2D::TransformType == "scale") r *= mTransformMatrix.matrix[0][0];
		//Matrix2D::TransformType = "none";
		Vec3 v(center.x, center.y);
		v = v * m;
		PixelPoint3D Center;
		Center.x = v.x();
		Center.y = v.y();
		Center.z = v.z();
		//Raster::drawCircle(pGeometry->x, pGeometry->y, pGeometry->r, pGeometry->color);
		g_Raster.drawCircle(Center.x, Center.y, r, pGeometry->mycolor);
		//g_Raster.drawCircle(center.x, center.y, r, pGeometry->mycolor);
		if (g_State.Fi11Mode == FILL_2D)
		{
			vector <PixelPoint> _pts(5);//drawPolygon函数参数是PixelPoint数组，_pts用于接收Point2D转化的PixelPoint数组
			_pts[0].x = Center.x-r; _pts[0].y = Center.y;
			_pts[2].x = Center.x+r; _pts[1].y = Center.y;
			_pts[3].x = Center.x; _pts[2].y = Center.y-r;
			_pts[1].x = Center.x; _pts[3].y = Center.y+r;
			_pts[4].x = Center.x - r; _pts[4].y = Center.y;
			if (g_State.DrawPolygonMethod == dpXScanline)
				//g_Raster.polygonScan(_pts.data(), ptsCount, p1);
				g_Raster.drawPolygon(_pts.data(), 5, pGeometry->mycolor);
		}
	}
	break;

	
	
	}
}

//渲染网格背景图
void Renderer2D::renderBackground()
{
	if (g_State.DrawMode!= dmPixel)
		g_Grid.drawGrid();
}

void Renderer2D::translate(double dx, double dy)
{
	m *= Matrix3::translate(dx, dy);
}


//旋转，逆时针为正， 以弧度为单位
void Renderer2D::rotate(double rad)
{
	m *= Matrix3::rotate(rad);
}

//定点旋转，逆时针为正， 以弧度为单位
void Renderer2D::rotate(double x, double y, double rad)
{
	Matrix3 m;
	m.makeTranslate(-x, -y);
	m *= m.rotate(rad);
	m *= m.makeTranslate(x, y);
	
}

//中心放缩
void Renderer2D::scale(double s)
{
	m *= Matrix3::scale(s,s);
}

//定点放缩
void Renderer2D::scale(double x, double y, double s)
{
	Matrix3 m;
	m.makeTranslate(-x, -y);
	m *= m.scale(s,s);
	m *= m.makeTranslate(x, y);
}

void Renderer2D::applyViewMatrix(Matrix3& m)
{
	this->m *= m;
}