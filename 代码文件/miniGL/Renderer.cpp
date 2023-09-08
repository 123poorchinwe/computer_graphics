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
	/*���shpͼ���ʱ���õ�*/
	double s = std::min(WWidth / w, WHeight / h);

	w = WWidth / s;
	h = WHeight / s;
	/*��shp����ƽ�ƺͷ���()��ʹ֮�ܹ�����Ļ��������ʾ*/
	m.makeTranslate(w / 2 - dx, h / 2 - dy);
	m *= Matrix3::scale(s, s);
	mTransformMatrix *= m;
}



//����
void Renderer2D::reset()
{
	setWindow(mOrigXCenter_map - mOrigWidth_map * 0.5, mOrigXCenter_map + mOrigWidth_map * 0.5,
		mOrigYCenter_map - mOrigHeight_map * 0.5, mOrigYCenter_map + mOrigHeight_map * 0.5);

	double w = getWindowWidth();
	double h = getWindowHeight();

	setViewport(mX_win * w, mY_win * h, mWidth_win * w, mHeight_win * h);
}
//�ӿڴ�С�ı�
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
//��Ⱦ���ݼ�
void Renderer2D::render(Dataset* pDataset)
{
	for (int i = pDataset->getLayerCount() - 1; i >= 0; --i)
	{
		Layer* pLayer = (*pDataset)[i];
		render(pLayer);
	}
}
//��Ⱦͼ��
void Renderer2D::render(Layer* pLayer)
{
	setPenColor(pLayer->layerColor);

	for (int i = 0, size = pLayer->getGeometryCount(); i < size; ++i)
	{
		render((*pLayer)[i]);
	}
}

//��ÿ��ͼ�ν�����Ⱦ
void Renderer2D::render(Geometry* pGeometryDef)
{
		
	switch (pGeometryDef->getGeomType())
	{
	case gtPoint:
	{
		PointGeometry* pGeometry = (PointGeometry*)pGeometryDef;
		
		Vec3 v(pGeometry->x, pGeometry->y);
		v *= m;
		setPixel(v.x(), v.y(), pGeometry->mycolor);//����
	}
	break;
	case gtPolyline:
	{
		PolylineGeometry* pGeometry = (PolylineGeometry*)pGeometryDef;//ת��Ϊʵ��ͼ������
		
		const vector<Point2D>& pts = pGeometry->getPts();//���ͼ�ζ��㼯��
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
			 //�������
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
		PolygonGeometry* pGeometry = (PolygonGeometry*)pGeometryDef;//ת��Ϊʵ��ͼ������
		const vector<Point2D>& pts = pGeometry->getPts();//���ͼ�ζ��㼯��
		size_t ptsCount = pts.size();
		vector <PixelPoint> _pts(ptsCount);//drawPolygon����������PixelPoint���飬_pts���ڽ���Point2Dת����PixelPoint����
		for (int i = 0; i < ptsCount; ++i)
		{
			Vec3 v(pts[i].x, pts[i].y);
			v *= m;
			_pts[i].x = v.x();
			_pts[i].y = v.y();
		}
		//drawPolygon(_pts.data(), ptsCount);
		
		//������߿�
		g_Raster.drawPolygonline(_pts.data(), ptsCount, pGeometry->mycolor);
		//���
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
		CircleGeometry* pGeometry = (CircleGeometry*)pGeometryDef;//ת��Ϊʵ��Բ��
		
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
			vector <PixelPoint> _pts(5);//drawPolygon����������PixelPoint���飬_pts���ڽ���Point2Dת����PixelPoint����
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

//��Ⱦ���񱳾�ͼ
void Renderer2D::renderBackground()
{
	if (g_State.DrawMode!= dmPixel)
		g_Grid.drawGrid();
}

void Renderer2D::translate(double dx, double dy)
{
	m *= Matrix3::translate(dx, dy);
}


//��ת����ʱ��Ϊ���� �Ի���Ϊ��λ
void Renderer2D::rotate(double rad)
{
	m *= Matrix3::rotate(rad);
}

//������ת����ʱ��Ϊ���� �Ի���Ϊ��λ
void Renderer2D::rotate(double x, double y, double rad)
{
	Matrix3 m;
	m.makeTranslate(-x, -y);
	m *= m.rotate(rad);
	m *= m.makeTranslate(x, y);
	
}

//���ķ���
void Renderer2D::scale(double s)
{
	m *= Matrix3::scale(s,s);
}

//�������
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