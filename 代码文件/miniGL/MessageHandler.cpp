#include <windows.h>
#include "MessageHandler.h"
#include "resource.h"
#include "Graphic.h"
#include "GeoDefine.h"
#include "GeometryFactory.h"
#include "Renderer2D.h"
#include "DialogHelper.h"
#include "LayerImporter.h"
#include "Vec3.h"
#include "Editor.h"
#include "GeoDefine3D.h"
#include "0_render3D.h"
#include"1_state.h"
#include <cmath>
#include"1_Rasterprint.h"
#include <stack>
#include "Selector.h"

enum OperationType {
	otNone, otSelect, otPan, otScaleOfCenter, otScaleOfFixedPoint,
	otDrawRectangle, otDrawRectangleOutline,
	otDrawLine, otDrawPolyline, otDrawPolygon, otDrawPolygonOutline,
	otDrawCircle, otDrawEllipse,
	ot3DRotate,otCai

};
Renderer3D::Rendertype g_Rt = Renderer3D::Rendertype::otwireframe;
Color ch_color = RED;
int cc = 0;
bool g_IsEditMode = false;//�Ƿ�༭ģʽ
OperationType g_OperationType = otPan;//��ǰ��������
Dataset g_Dataset;//���ݼ�
Layer* g_pLayer = NULL;//��ǰͼ�㣬һ��ָ����������û����Ʋ�
Renderer2D g_Renderer;//2D��Ⱦ��
//Selector g_Selector(&g_Renderer);//2Dѡ����
//Editor2D g_Editor(&g_Renderer);//2D�༭��
Editor g_Editor;
Renderer3D g_Renderer3D;
bool g_3dMode = false;//�Ƿ�����ά��Ⱦģʽ
bool g_zj;//�Ƿ�����ͶӰ
Layer* g;
GroupNode g_root;//���ڵ�
Renderer2D p_Renderer;
boolean Reset = false;
//��������
void refreshMenuState();//���²˵�״̬
void boolpre();
Layer* openLayer();
Geometry* createGeometry(OperationType operationType, vector<PixelPoint>& pts);


///����˵���Ϣ
void handleMenuMessage(int menuID)
{
	switch (menuID)
	{
	case ID_ROAM_MODE://���ģʽ
		g_IsEditMode = !g_IsEditMode;
		g_OperationType = otSelect;
		setRubberMode(rmNone);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_RECT://������
		g_State.Fi11Mode = FILL_NONE;
		g_OperationType = otDrawRectangle;
		g_IsEditMode = true;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_LINE://����
		g_OperationType = otDrawLine;
		g_IsEditMode = true;
		setRubberMode(rmLine);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_POLYLINE://������		
		g_OperationType = otDrawPolyline;
		g_IsEditMode = true;
		setRubberMode(rmPolyline);
		setCursor(csCross);
		break;
	case ID_2D_DRAW_POLYGON://�������
		g_State.Fi11Mode = FILL_NONE;
		g_OperationType = otDrawPolygon;
		g_IsEditMode = true;
		setRubberMode(rmPolygon);
		setCursor(csCross);
		break;
	case ID_2D_DRAW_ELLI://��Բ��
		g_OperationType = otDrawCircle;
		g_IsEditMode = true;
		setRubberMode(rmPolygon);
		setCursor(csCross);
		break;
	case ID_Draw_ELLE://����Բ
		g_OperationType = otDrawEllipse;
		g_IsEditMode = true;
		setRubberMode(rmPolygon);
		setCursor(csCross);
		break;
	case ID_SELECT://ѡ��
		g_OperationType = otSelect;
		setRubberMode(rmNone);
		setCursor(csArrow);
		break;
	 //2D��3Dģʽת��
	case ID_CONTENT_2D://2D��ģʽ
		 g_3dMode = false;
		 refreshWindow();
		break;
	case ID_CONTENT_3D://3D��ģʽ��ֱ����ʾ3Dģ��
		 g_3dMode = true;
		 setRubberMode(rmNone);
		break;
	case ID_zhengjiao://����ͶӰ
		g_Rt = Renderer3D::Rendertype::otwireframe;
		g_3dMode = true;
		setRubberMode(rmNone);
		g_zj =true;
		boolpre();
		refreshWindow();
		break;
	case ID_toushe://͸��ͶӰ
		g_Rt = Renderer3D::Rendertype::otwireframe;
		g_3dMode = true;
		setRubberMode(rmNone);
		g_zj = false;
		boolpre();
		refreshWindow();
		break;
	case ID_LIGHT_1://����ģ��
		g_Rt = Renderer3D::Rendertype::otfiller;
		refreshWindow();
		break;
	case ID_3D_32788:
		// ��յ�ǰ����
		Reset = true;
		g_pLayer->DeleteGeometry();
		//g_pLayer->clerarCanvas();
		Reset = false;
		break;
		break;
	//�޳�ģʽ
	case ID_CULL_NONE://���޳�
		g_State.CullMode = CULL_NONE;
		g_Rt = Renderer3D::Rendertype::otwireframe;
		refreshWindow();
		break;
	case ID_CULL_CLOCKWISE://ǰ�����޳�
		g_State.CullMode = CULL_CLOCKWISE;
		refreshWindow();
		break;
	case ID_CULL_COUNTCLOCKWISE://�������޳�
		g_State.CullMode = CULL_COUNTCLOCKWISE;
		refreshWindow();
		break;
	//����ģʽ
	case ID_dmPixel://����
		g_State.DrawMode = dmPixel;
		refreshWindow();
		break;
	case ID_dmGrid://����
		g_State.DrawMode = dmGrid;
		refreshWindow();
		break;
	case ID_black://���ıʵ���ɫ������Ϊ�Ѿ����˵�ɫ�壬���Ծ�û������
		ch_color = BLACK;
		cc = 0;
		refreshWindow();
		break;
	case ID_blue:
		ch_color = BLUE;
		cc = 0;
		refreshWindow();
		break;
	case ID_red:
		ch_color = RED;
		cc = 0;
		refreshWindow();
		break;
	case ID_green:
		ch_color = GREEN;
		cc = 0;
		refreshWindow();
		break;
	case ID_exgrid:
		g_State.DrawMode = dmPixel;
		refreshWindow();
		break;
    //��դ����ģʽ
	case ID_dlDDA:
		g_State.DrawMode = dmPixel;
		g_State.DrawLineMethod = dlDDA;
		refreshWindow();
		break;
	case ID_dlMidBresenham:
		g_State.DrawMode = dmPixel;
		g_State.DrawLineMethod = dlMidBresenham;
		refreshWindow();
		break;
	///�ü�ģʽ
	case ID_32848://�е�
		g_OperationType = otCai;
		g_State.ClipMethod = MidPoint;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		refreshWindow();
		break;
	case ID_32851:
		g_OperationType = otCai;//�����
		g_State.ClipMethod = Sutherland_Hodgeman;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		refreshWindow();
		break;
	case ID_32849:
		g_OperationType = otCai;//����ü�
		g_State.ClipMethod = CohenSutherland;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		refreshWindow();
		break;
	//��ɫѡ��
	case ID_PEN_COLOR://������ɫ
	{
		cc = 1;
		if (g_3dMode)
		{
			Color color = getPenColor();
			if (DialogHelper::selectColor(color, color))
			{
				setPenColor(color);
				refreshWindow();
			}
		}
		else
		{
			if (DialogHelper::selectColor(g_pLayer->layerColor, g_pLayer->layerColor))
			{
				refreshWindow();
			}
		}
		break;
	}
	case ID_BACK_COLOR://������ɫ
	{
		Color color = getBackColor();
		if (DialogHelper::selectColor(color, color))
		{
			setBackColor(color);
			refreshWindow();
		}
		break;
	}
	//���ģʽ
	case ID_FILL_NONE:
	{
		g_State.Fi11Mode = FILL_NONE;
		refreshWindow();
		break;
	}
	//�˳�����

	case IDM_EXIT:
		
		init();
		
		break;
	//X���Ա߱�ɨ����
	case ID_dpXScanline:
	{
		g_State.Fi11Mode = FILL_2D;
		g_State.DrawPolygonMethod = dpXScanline;
		refreshWindow();
		break;
	}
	//�������
	case ID_Boundfill:
	{
		g_State.Fi11Mode = FILL_2D;
		g_State.DrawPolygonMethod = dpBoundFill;
		refreshWindow();
		break;
	}
	//���Ա߱�z_buffer
	case ID_dpAET:
	{
		g_State.Fi11Mode = FILL_3D;
		refreshWindow();
		break;
	}

	//��һ��ͼ��
	case ID_OPEN_LAYER:
	{
		Layer* pLayer = openLayer();
		if (pLayer)
		{
			if (g_Dataset.getLayerCount() == 1)//��ӵ�һ��ͼ��ʱ���ó�ʼ�任����ͱ�����
			{
				double x_center = /*-*/pLayer->envelop.centerX();
				double y_center = /*-*/pLayer->envelop.centerY();

				g_Renderer.init(x_center, y_center, pLayer->envelop.width(), pLayer->envelop.height(),
					 getWindowWidth(), getWindowHeight());
			}
			g_Dataset.addLayer(pLayer);
			refreshWindow();
		}
		break;
	}
	//?case 
	}
	refreshMenuState();//ˢ�²˵�״̬

}
Matrix3 mm;
///��������Ϣ
void handleKeyMessage(int key)
{
#pragma region 2D
	if (!g_3dMode)
	{
		double pan = 10.0;
		double angle = DEG_RAD(30);

		Matrix3 m;
		switch (key)
		{
		case VK_UP: // ��һ�У��Ϲ��
			refreshWindow();

		case VK_DOWN:
			if (key == VK_DOWN)pan = -pan;
			m.makeTranslate(0, pan);
			break;
		case VK_LEFT:
			refreshWindow();

		case VK_RIGHT:
			if (key == VK_LEFT) pan = -pan;
			if (key == VK_RIGHT) angle = -angle;
			if (isCtrlKeyPressed())                  //��סctrl�����ĵ���ת
			{
				int x = getWindowWidth() / 2;
				int y = getWindowHeight() / 2;
				m.makeTranslate(-x, -y);
				m *= Matrix3::rotate(angle);
				m *= Matrix3::translate(x, y);
				g_Renderer.mTransformMatrix *= m;
			}
			else if (isShiftKeyPressed())           //��סshift����������󶨵���ת
			{
				int x0 = getWindowWidth() / 2;
				int y0 = 0;
				m.makeTranslate(-x0, -y0);
				m *= Matrix3::rotate(angle);
				m *= Matrix3::n_translate(-x0, -y0);
			}
			else
				m.makeTranslate(pan, 0);
			refreshWindow();
			break;
		default:
			return;
		}

		if (g_IsEditMode)
		{
			g_Editor.apply(&g_Dataset, m);
		}
		else
		{
			g_Renderer.applyViewMatrix(m);
		}

		refreshWindow();
		return;
	}
#pragma endregion
#pragma region 3D
	double rotate_x = PI / 16;
	double rotate_y = PI / 16;
	double rotate_z = PI / 16;
	double step = 10.0;
	double h = 0.1;
	Matrix4 m;
	switch (key)
	{
	    case VK_UP:
	    {
		  if (isShiftKeyPressed())
			  m.makeRotate(-rotate_x, 0);            //��x��(ƽ�ģ���ת
		  else if (isCtrlKeyPressed())
			  m.makeMori();                          //x,zƽ��Գ�	 
		  else
		      m.makeTranslate(0, step, 0);
		  //��y�ᣨ���ϣ�ƽ��
		  refreshWindow();
			
	    }
		break;
		case VK_DOWN:
		{
			if (isShiftKeyPressed())
				m.makeRotate(rotate_x, 0);
			else if (isCtrlKeyPressed())
				m.makecut(h);                       //��z�����뿪y�����
			else
				m.makeTranslate(0, -step, 0);
			refreshWindow();
		}
		break;
		case VK_LEFT:
		{
			if (isShiftKeyPressed())
				m.makeRotate(rotate_y, 1);           //��y����ת
			else if (isCtrlKeyPressed())
			{
				Vec3 a(0, 0, 0), b(1, 1, 1);
				Vec3 c = b - a;
				c = c.normalize();

				m.makeTranslate(-c[0], -c[1], -c[2]);

				double v = sqrt(c[0] * c[0] + c[2] * c[2]);
				double p1 = c[0] / v;
				double p2 = c[2] / v;
				double ja = acos(p1);

				m *= Matrix4::Rotate(ja, 1);

				double jb = acos(v);
				m *= Matrix4::Rotate(-jb, 2);

				m *= Matrix4::Rotate(rotate_x, 0);

				m *= Matrix4::Rotate(jb, 2);
				m *= Matrix4::Rotate(-ja, 1);
				m *= Matrix4::translate(c[0], c[1], c[2]);
				refreshWindow();
			}
				//m.makeRotate(rotate_z, 2);           //��z����ת
			else if (isAltKeyPressed())              //�ƹ̶��ᣨ1,1,1����ת
			{
				Vec3 a(0, 0, 0), b(1, 1, 1);
				Vec3 c = b - a;
				c = c.normalize();

				m.makeTranslate(-c[0], -c[1], -c[2]);

				double v = sqrt(c[0] * c[0] + c[2] * c[2]);
				double p1 = c[0] / v;
				double p2 = c[2] / v;
				double ja = acos(p1);

				m *= Matrix4::Rotate(ja, 1);
				
				double jb = acos(v);
				m *= Matrix4::Rotate(-jb, 2);

				m *= Matrix4::Rotate(rotate_x, 0);

				m *= Matrix4::Rotate(jb, 2);
				m *= Matrix4::Rotate(-ja, 1);
				m *= Matrix4::translate(c[0], c[1], c[2]);
				refreshWindow();
			}
			else
				m.makeTranslate(-step, 0, 0);
		}
		break;
		case VK_RIGHT:
		{
			if (isShiftKeyPressed())  //��ת
				m.makeRotate(-rotate_y, 1);
			else if (isCtrlKeyPressed())
				m.makeRotate(-rotate_z, 2);
			else
				m.makeTranslate(step, 0, 0);
			refreshWindow();
		}
		break;
		default:
			return;

	}
	g_Renderer3D.g_viewMatrix *= m;                                //ÿ���ۻ�����ͼ�任����
	refreshWindow();
#pragma endregion
}





///���������Ϣ
void handleMouseMessage(int message, int x, int y, int det)
{
	DPtToLPt(x, y, x, y);                             //�����豸����תΪ�����߼�����

	static Point2D pt1;
	static bool dragging = false;
	static bool draw_ok = true;
	Matrix3 m;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		//if (g_OperationType == otPan)//��ǰ��������
		{
			pt1.x = x;
			pt1.y = y;
			dragging = true;
			
		}
		refreshWindow();
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (dragging && draw_ok)
		{
			if (pt1.x != x || pt1.y != y)
			{    
				//Matrix3 m;
				//m.makeTranslate(x - pt1.x, -y + pt1.y);//�豸����
				m.makeTranslate(x - pt1.x, y - pt1.y);//�߼�����
				pt1.x = x;
				pt1.y = y;
				mm *= m;
				g_Renderer.mTransformMatrix *= m;
				g_Renderer.applyViewMatrix(m);
				//updateSelectSet(m);
				refreshWindow();
			}
		}

		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (!g_3dMode)
		{
			if (isCtrlKeyPressed())
			{
			
				//��������
				x = getWindowWidth();
				y = getWindowHeight();
				double d = det > 0 ? 0.9 : 1.0 / 0.9;

			
				m.makeTranslate(-x, -y);
				m *= Matrix3::scale(d, d);
				m *= Matrix3::n_translate(-x, -y);     //�����

				g_Renderer.applyViewMatrix(m);
				
			}
			double d = det > 0 ? 0.9 : 1.0 / 0.9;      //�ȱȷ���
			int x0 = getWindowWidth() / 2;
			int y0 = getWindowHeight() / 2;
			m.makeTranslate(-x0, -y0);
			m *= Matrix3::scale(d, d);
			m *= Matrix3::translate(x0, y0);
			g_Renderer.mTransformMatrix *= m;
			//mm *= m;
			g_Renderer.applyViewMatrix(m);
			//updateSelectSet(m);
			
		}
		else
		{
			if (isCtrlKeyPressed())                              //�ȱ�������
			{
				double d = det > 0 ? 0.9 : 1.0 / 0.9;
				g_Renderer3D.g_viewMatrix *= Matrix4::scale(d, d, d);
			}

			double d = det > 0 ? 10 : -10;
			g_Renderer3D.g_viewMatrix *= Matrix4::translate(0, 0, d);        //�����൱���ӵ���z����ƽ��
		}
		

		refreshWindow();
		break;
	}

	case WM_LBUTTONUP:
	{
		
		refreshWindow();
		dragging = false;
		RubberMode rm = getRubberMode();
		if (rm == rmNone || rm == rmPolygon) break;
		int c = getRubberPointCount();
		if (c == 2)
		{
			PixelPoint pt1, pt2;
			getRubberPoints(pt1, pt2);
			switch (g_OperationType)
			{
			case otCai:
				/// <summary>
				/// �ü��Ľ���
				/// </summary>
				/// <param name="message"></param>
				/// <param name="x"></param>
				/// <param name="y"></param>
				/// <param name="det"></param>
				if (g_State.ClipMethod = CohenSutherland)
					Raster:CohenSutherland( pt1,  pt2, g_pLayer);
				if (g_State.ClipMethod = MidPoint)
					Raster::MidPointClipAlgorithm(pt1, pt2, g_pLayer);
				if (g_State.ClipMethod = Sutherland_Hodgeman)
					Raster::Sutherland_Hodgeman(pt1, pt2, g_pLayer);
			
			}
			refreshWindow();
		}
	}
	case WM_RBUTTONUP:
	{
		dragging = false;
		if (g_3dMode) return;

		if (getRubberMode() == rmNone) return;//����Ƥ��ģʽ���˳�

		int c = getRubberPointCount();
		if (c < 2) return;//��Ƥ�߶�����<2 ,����������Чͼ�Σ��˳�

		vector<PixelPoint> pts(c);//ptsΪ��̬���飬�����СΪc
		getRubberPoints(pts.data());//pts������Ƥ�߶��㼯��
	    
		//���ͼ��ǰ�����ۻ��任����������
		m = g_Renderer.mTransformMatrix.makeInverseMatrix();
		//����ͼ�ε��������꣬�����仯Ϊʵ������
		for (int i = 0; i < pts.size(); i++) 
		{
			Vec3 v(pts[i].x, pts[i].y);
		
			v *= m;
			pts[i].x = v.x();
			pts[i].y = v.y();
		}
		
		//��Ƥ�߲�������ʱ�Żᴴ��ͼ�ζ��������ж���Ƥ�߲����Ƿ����
		switch (g_OperationType)
		{
		case otDrawLine:
			draw_ok = false;
		case otDrawCircle:
			draw_ok = false;
		case otDrawRectangle:
			draw_ok = false;
		case otDrawRectangleOutline:
			draw_ok = false;
	
		case otDrawEllipse:
			draw_ok = false;
			
		
		case otDrawPolyline:
			draw_ok = false;
		case otDrawPolygon:
			draw_ok = false;
		case otDrawPolygonOutline:
			if (message == WM_LBUTTONUP)return;//������������Line��Rectangeģʽ������־
			draw_ok = TRUE;
			break;
		default:
			refreshWindow();
			return;

		}

		//��Ƥ�߲���������������Ƥ�߶�������ͼ��
		Geometry* pGeometry = createGeometry(g_OperationType, pts);
		if (pGeometry)
		{
			g_pLayer->addGeometry(pGeometry);
			draw_ok = TRUE;
			refreshWindow();
		}
	}
	draw_ok = TRUE;
	break;
	}
}


void display()
{
	setYUp(true);                                   //y������
	setOrig( 0, getWindowHeight());                 //�߼�����Ϊ���½�

	if (g_State.DrawMode == dmGrid)
		{
		g_Renderer.renderBackground();
		}
	if (!g_3dMode)
	{
		g_Renderer.render(&g_Dataset);
		refreshWindow();
		
	}
	else
	{
		g_Renderer3D.rendererSceneNode(&g_root, Matrix4(),g_Rt);
	}
	//g_Renderer.renderSelectionSet(g_Selector.getSelectionSet());//��Ⱦѡ��
}

///���ݲ�������operationType�Ͷ��㼯��pts�������ʵ�ͼ�ζ���
Geometry* createGeometry(OperationType operationType, vector<PixelPoint>& pts)
{
	int count = pts.size();
	if (count < 2) return NULL;

	PixelPoint* pData = pts.data();

	///��ͬ�������Ϳ�������ͬһ���͵�ͼ�Σ�����Rectangeģʽ���ɵ�ͼ��Ҳ�Ƕ����
	switch (operationType)
	{
	case otDrawLine:
		return GeometryFactory::createPolylineGeometry(pData, count);
	case otDrawRectangle:
		return GeometryFactory::creatRectangleGeometry(pData, count);
	case otDrawRectangleOutline:
		return GeometryFactory::creatRectangleOutlineGeometry(pData, count);
	case otDrawPolyline:
		return GeometryFactory::createPolylineGeometry(pData, count);
	case otDrawPolygon:
		return GeometryFactory::createPolygonGeometry(pData, count);
	case otDrawPolygonOutline:
		return GeometryFactory::createPolygonOutlineGeometry(pData, count);
	case otDrawCircle:
		return GeometryFactory::createCircleGeometry(pData, count);
	case otDrawEllipse:
		return GeometryFactory::createEllipseGeometry(pData, count);
	}
	return NULL;
}

//ѡ�񲢴�ͼ��
Layer* openLayer()
{
	char path[MAX_PATH] = { 0 };
	if (DialogHelper::selectSingleFile("", "shp", path, MAX_PATH))
	{
		return LayerImporter::importShpLayer(path);
	}
	return NULL;
}

/// <summary>
/// ���²˵�״̬
/// </summary>
void refreshMenuState()
{
	void checkMenuItem(int menuID, bool checked);
	void setMenuItemText(int menuID, const char* text);

	//�޸Ĳ˵�������
	//setMenuItemText(ID_ROAM_MODE, g_IsEditMode =="���ģʽ������е��༭ģʽ��");

	//�޸Ĳ˵����״̬
	checkMenuItem(ID_2D_DRAW_RECT, g_OperationType == otDrawRectangle);
	checkMenuItem(ID_2D_DRAW_LINE, g_OperationType == otDrawLine);
	checkMenuItem(ID_2D_DRAW_POLYLINE, g_OperationType == otDrawPolyline);
	checkMenuItem(ID_2D_DRAW_POLYGON, g_OperationType == otDrawPolygon);
    checkMenuItem(ID_2D_DRAW_ELLI,g_OperationType == otDrawCircle);
	checkMenuItem(ID_Draw_ELLE, g_OperationType == otDrawEllipse);
	checkMenuItem(ID_SELECT, g_OperationType == otSelect);
	checkMenuItem(ID_CULL_NONE, g_State.CullMode == CULL_NONE);
}

void setMenuItemText(int menuID, const char* text)
{
	extern HWND g_hWnd;
	HMENU hMenu = GetMenu(g_hWnd);
	ModifyMenuA(hMenu, menuID, MF_BYCOMMAND | MF_STRING, menuID, text);
}

void checkMenuItem(int menuID, bool checked)
{
	extern HWND g_hWnd;
	HMENU hMenu = GetMenu(g_hWnd);
	CheckMenuItem(hMenu, menuID, checked ? MF_CHECKED : MF_UNCHECKED);
}

#pragma region less-used 
double g_fovy = 60, g_zNear = 10, g_zFar = 1600;

///�����ڴ�С�仯��Ϣ
void sized(int cx, int cy)
{
	if (cx == 0 || cy == 0)return;
	double aspectRatio = (double)cx / cy;
	boolpre();
	
	//��θ���ͶӰ����ֱ�ӵõ�Զ��ƽ�����
	//
	
	//g_Renderer.setViewport(0, 0, cx, cy);
}

///��ʼ��
void initialize()
{
	g_pLayer = new Layer();
	g_Dataset.addLayer(g_pLayer);	

	g_OperationType = ot3DRotate;

	int width = getWindowWidth();
	int height = getWindowHeight();
	//g_Renderer.setWindow(0, w, 0, h);
	if (width == 0 || height == 0) return;
	//�ӿڱ任
	g_Renderer3D.g_viewport.makeScale(0.5, 0.5, 1.0);
	g_Renderer3D.g_viewport*= Matrix4::translate(0.5, 0.5, 0.0);
	////͸��ͶӰ
	//boolpre();
	
	//g_Renderer3D.setViewport(width, height);
	
	//�۲�����ϵ����������ϵ�ж��壬����۲�����ϵ
	
	Vec3 eye(600, 600, 600), center(0, 0, 0), up(0, 1, 0);
	g_Renderer3D.g_viewMatrix.makeLookAt(eye, center, up);
		
	//g_Renderer3D.lookAt(eye, center, up);
	
	//ģ�;���
	//����ʹ�õ���������ϵ��λΪ���أ��任�����ʱy��Ҫת��Ϊ�ӿ���������
	//g_pMesh = Box3D().toMesh();//ģ�ͣ�����һ�����ӣ�-1��1���ٱ任һ��

	//g_modelMatrix.makeScale(100, 100, 100);
	//g_modelMatrix *= Matrix4::translate(300, 300, -100);
	
	g_Renderer3D.buildScene(&g_root);
	//g_Renderer3D.render(g_pMesh);
	refreshMenuState();
}
//ͶӰ��ʽ
void boolpre()
{
	int width = getWindowWidth();
	int height = getWindowHeight();
	double aspectRatio = (double)width / height;
	if (!g_zj)
		g_Renderer3D.g_projectMatrix.makePerspective(g_fovy, aspectRatio, g_zNear, g_zFar);
	//����ͶӰ
	else
		g_Renderer3D.g_projectMatrix.makeortho(700, 600, g_zNear, g_zFar);
	    //g_Renderer3D.g_projectMatrix.ortho(0,width,0,height,  g_zNear, g_zFar);
	
}


///���ٴ���ʱ��������
void destroy()
{
	//�˴��Ĵ���ֱ��д���˳���ť���ˣ�ֱ�Ӳ���init()��ʼ��
}


#pragma endregion

#pragma region handleMessage

///������Ϣ����ڵ�
LRESULT handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND://�˵���Ϣ
	{
		int menuID = LOWORD(wParam);
		handleMenuMessage(menuID);
		return FALSE;
	}
	case WM_PAINT://������Ϣ
		display();
		return FALSE;
	case WM_CREATE:
		init((unsigned)hWnd);
		initialize();
		return TRUE;
	case WM_DESTROY:
		destroy();
		PostQuitMessage(0);
		return TRUE;
	case WM_KEYDOWN://������Ϣ
	case WM_SYSKEYDOWN:
	{
		int key = wParam;//��ð�������
		handleKeyMessage(key);
		return FALSE;
	}
	case WM_MOUSEMOVE://����ƶ���Ϣ
	case WM_LBUTTONDOWN://������������Ϣ	
	case WM_LBUTTONUP://������������Ϣ
	case WM_RBUTTONUP://����Ҽ�������Ϣ
	case WM_RBUTTONDOWN://����Ҽ�������Ϣ
	case WM_LBUTTONDBLCLK://���˫����Ϣ
	case WM_MOUSEWHEEL://��������Ϣ
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		int det = message == WM_MOUSEWHEEL ? (short)HIWORD(wParam) : 0;
		handleMouseMessage(message, x, y, det);
		return FALSE;
	}
	case WM_SIZE://���ڴ�С�仯��Ϣ
	{
		UINT cx = LOWORD(lParam);
		UINT cy = HIWORD(lParam);
		sized(cx, cy);
		return TRUE;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

#pragma endregion





