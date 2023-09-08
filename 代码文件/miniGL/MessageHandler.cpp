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
bool g_IsEditMode = false;//是否编辑模式
OperationType g_OperationType = otPan;//当前操作类型
Dataset g_Dataset;//数据集
Layer* g_pLayer = NULL;//当前图层，一般指向最上面的用户绘制层
Renderer2D g_Renderer;//2D渲染器
//Selector g_Selector(&g_Renderer);//2D选择器
//Editor2D g_Editor(&g_Renderer);//2D编辑器
Editor g_Editor;
Renderer3D g_Renderer3D;
bool g_3dMode = false;//是否是三维渲染模式
bool g_zj;//是否正交投影
Layer* g;
GroupNode g_root;//根节点
Renderer2D p_Renderer;
boolean Reset = false;
//函数声明
void refreshMenuState();//更新菜单状态
void boolpre();
Layer* openLayer();
Geometry* createGeometry(OperationType operationType, vector<PixelPoint>& pts);


///处理菜单消息
void handleMenuMessage(int menuID)
{
	switch (menuID)
	{
	case ID_ROAM_MODE://浏览模式
		g_IsEditMode = !g_IsEditMode;
		g_OperationType = otSelect;
		setRubberMode(rmNone);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_RECT://画矩形
		g_State.Fi11Mode = FILL_NONE;
		g_OperationType = otDrawRectangle;
		g_IsEditMode = true;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_LINE://划线
		g_OperationType = otDrawLine;
		g_IsEditMode = true;
		setRubberMode(rmLine);
		setCursor(csArrow);
		break;
	case ID_2D_DRAW_POLYLINE://划折线		
		g_OperationType = otDrawPolyline;
		g_IsEditMode = true;
		setRubberMode(rmPolyline);
		setCursor(csCross);
		break;
	case ID_2D_DRAW_POLYGON://画多边形
		g_State.Fi11Mode = FILL_NONE;
		g_OperationType = otDrawPolygon;
		g_IsEditMode = true;
		setRubberMode(rmPolygon);
		setCursor(csCross);
		break;
	case ID_2D_DRAW_ELLI://画圆形
		g_OperationType = otDrawCircle;
		g_IsEditMode = true;
		setRubberMode(rmPolygon);
		setCursor(csCross);
		break;
	case ID_Draw_ELLE://画椭圆
		g_OperationType = otDrawEllipse;
		g_IsEditMode = true;
		setRubberMode(rmPolygon);
		setCursor(csCross);
		break;
	case ID_SELECT://选择
		g_OperationType = otSelect;
		setRubberMode(rmNone);
		setCursor(csArrow);
		break;
	 //2D、3D模式转换
	case ID_CONTENT_2D://2D的模式
		 g_3dMode = false;
		 refreshWindow();
		break;
	case ID_CONTENT_3D://3D的模式，直接显示3D模型
		 g_3dMode = true;
		 setRubberMode(rmNone);
		break;
	case ID_zhengjiao://正交投影
		g_Rt = Renderer3D::Rendertype::otwireframe;
		g_3dMode = true;
		setRubberMode(rmNone);
		g_zj =true;
		boolpre();
		refreshWindow();
		break;
	case ID_toushe://透视投影
		g_Rt = Renderer3D::Rendertype::otwireframe;
		g_3dMode = true;
		setRubberMode(rmNone);
		g_zj = false;
		boolpre();
		refreshWindow();
		break;
	case ID_LIGHT_1://光照模型
		g_Rt = Renderer3D::Rendertype::otfiller;
		refreshWindow();
		break;
	case ID_3D_32788:
		// 清空当前画布
		Reset = true;
		g_pLayer->DeleteGeometry();
		//g_pLayer->clerarCanvas();
		Reset = false;
		break;
		break;
	//剔除模式
	case ID_CULL_NONE://不剔除
		g_State.CullMode = CULL_NONE;
		g_Rt = Renderer3D::Rendertype::otwireframe;
		refreshWindow();
		break;
	case ID_CULL_CLOCKWISE://前向面剔除
		g_State.CullMode = CULL_CLOCKWISE;
		refreshWindow();
		break;
	case ID_CULL_COUNTCLOCKWISE://后向面剔除
		g_State.CullMode = CULL_COUNTCLOCKWISE;
		refreshWindow();
		break;
	//绘制模式
	case ID_dmPixel://像素
		g_State.DrawMode = dmPixel;
		refreshWindow();
		break;
	case ID_dmGrid://网格
		g_State.DrawMode = dmGrid;
		refreshWindow();
		break;
	case ID_black://更改笔的颜色，但因为已经挂了调色板，所以就没有运用
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
    //光栅化线模式
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
	///裁剪模式
	case ID_32848://中点
		g_OperationType = otCai;
		g_State.ClipMethod = MidPoint;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		refreshWindow();
		break;
	case ID_32851:
		g_OperationType = otCai;//多边形
		g_State.ClipMethod = Sutherland_Hodgeman;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		refreshWindow();
		break;
	case ID_32849:
		g_OperationType = otCai;//编码裁剪
		g_State.ClipMethod = CohenSutherland;
		setRubberMode(rmRectangle);
		setCursor(csArrow);
		refreshWindow();
		break;
	//颜色选择
	case ID_PEN_COLOR://画笔颜色
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
	case ID_BACK_COLOR://背景颜色
	{
		Color color = getBackColor();
		if (DialogHelper::selectColor(color, color))
		{
			setBackColor(color);
			refreshWindow();
		}
		break;
	}
	//填充模式
	case ID_FILL_NONE:
	{
		g_State.Fi11Mode = FILL_NONE;
		refreshWindow();
		break;
	}
	//退出程序

	case IDM_EXIT:
		
		init();
		
		break;
	//X活性边表扫描线
	case ID_dpXScanline:
	{
		g_State.Fi11Mode = FILL_2D;
		g_State.DrawPolygonMethod = dpXScanline;
		refreshWindow();
		break;
	}
	//区域填充
	case ID_Boundfill:
	{
		g_State.Fi11Mode = FILL_2D;
		g_State.DrawPolygonMethod = dpBoundFill;
		refreshWindow();
		break;
	}
	//活性边表法z_buffer
	case ID_dpAET:
	{
		g_State.Fi11Mode = FILL_3D;
		refreshWindow();
		break;
	}

	//打开一个图层
	case ID_OPEN_LAYER:
	{
		Layer* pLayer = openLayer();
		if (pLayer)
		{
			if (g_Dataset.getLayerCount() == 1)//添加第一个图层时设置初始变换矩阵和比例尺
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
	refreshMenuState();//刷新菜单状态

}
Matrix3 mm;
///处理按键消息
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
		case VK_UP: // 上一行，上光标
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
			if (isCtrlKeyPressed())                  //按住ctrl绕中心点旋转
			{
				int x = getWindowWidth() / 2;
				int y = getWindowHeight() / 2;
				m.makeTranslate(-x, -y);
				m *= Matrix3::rotate(angle);
				m *= Matrix3::translate(x, y);
				g_Renderer.mTransformMatrix *= m;
			}
			else if (isShiftKeyPressed())           //按住shift，利用逆矩阵定点旋转
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
			  m.makeRotate(-rotate_x, 0);            //绕x轴(平的）旋转
		  else if (isCtrlKeyPressed())
			  m.makeMori();                          //x,z平面对称	 
		  else
		      m.makeTranslate(0, step, 0);
		  //沿y轴（朝上）平移
		  refreshWindow();
			
	    }
		break;
		case VK_DOWN:
		{
			if (isShiftKeyPressed())
				m.makeRotate(rotate_x, 0);
			else if (isCtrlKeyPressed())
				m.makecut(h);                       //沿z方向离开y轴错切
			else
				m.makeTranslate(0, -step, 0);
			refreshWindow();
		}
		break;
		case VK_LEFT:
		{
			if (isShiftKeyPressed())
				m.makeRotate(rotate_y, 1);           //绕y轴旋转
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
				//m.makeRotate(rotate_z, 2);           //绕z轴旋转
			else if (isAltKeyPressed())              //绕固定轴（1,1,1）旋转
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
			if (isShiftKeyPressed())  //旋转
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
	g_Renderer3D.g_viewMatrix *= m;                                //每次累积到视图变换矩阵
	refreshWindow();
#pragma endregion
}





///处理鼠标消息
void handleMouseMessage(int message, int x, int y, int det)
{
	DPtToLPt(x, y, x, y);                             //窗口设备坐标转为窗口逻辑坐标

	static Point2D pt1;
	static bool dragging = false;
	static bool draw_ok = true;
	Matrix3 m;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		//if (g_OperationType == otPan)//当前操作类型
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
				//m.makeTranslate(x - pt1.x, -y + pt1.y);//设备坐标
				m.makeTranslate(x - pt1.x, y - pt1.y);//逻辑坐标
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
			
				//定点缩放
				x = getWindowWidth();
				y = getWindowHeight();
				double d = det > 0 ? 0.9 : 1.0 / 0.9;

			
				m.makeTranslate(-x, -y);
				m *= Matrix3::scale(d, d);
				m *= Matrix3::n_translate(-x, -y);     //逆矩阵

				g_Renderer.applyViewMatrix(m);
				
			}
			double d = det > 0 ? 0.9 : 1.0 / 0.9;      //等比放缩
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
			if (isCtrlKeyPressed())                              //等比例放缩
			{
				double d = det > 0 ? 0.9 : 1.0 / 0.9;
				g_Renderer3D.g_viewMatrix *= Matrix4::scale(d, d, d);
			}

			double d = det > 0 ? 10 : -10;
			g_Renderer3D.g_viewMatrix *= Matrix4::translate(0, 0, d);        //缩放相当于视点在z轴上平移
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
				/// 裁剪的交互
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

		if (getRubberMode() == rmNone) return;//非橡皮线模式，退出

		int c = getRubberPointCount();
		if (c < 2) return;//橡皮线顶点数<2 ,不能生成有效图形，退出

		vector<PixelPoint> pts(c);//pts为动态数组，数组大小为c
		getRubberPoints(pts.data());//pts保存橡皮线顶点集合
	    
		//组成图形前乘以累积变换矩阵的逆矩阵
		m = g_Renderer.mTransformMatrix.makeInverseMatrix();
		//创建图形的像素坐标，逆矩阵变化为实际坐标
		for (int i = 0; i < pts.size(); i++) 
		{
			Vec3 v(pts[i].x, pts[i].y);
		
			v *= m;
			pts[i].x = v.x();
			pts[i].y = v.y();
		}
		
		//橡皮线操作结束时才会创建图形对象，以下判断橡皮线操作是否结束
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
			if (message == WM_LBUTTONUP)return;//鼠标左键弹起是Line或Rectange模式结束标志
			draw_ok = TRUE;
			break;
		default:
			refreshWindow();
			return;

		}

		//橡皮线操作结束，根据橡皮线顶点生成图形
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
	setYUp(true);                                   //y轴向上
	setOrig( 0, getWindowHeight());                 //逻辑坐标为左下角

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
	//g_Renderer.renderSelectionSet(g_Selector.getSelectionSet());//渲染选择集
}

///根据操作类型operationType和顶点集合pts创建合适的图形对象
Geometry* createGeometry(OperationType operationType, vector<PixelPoint>& pts)
{
	int count = pts.size();
	if (count < 2) return NULL;

	PixelPoint* pData = pts.data();

	///不同操作类型可以生成同一类型的图形，比如Rectange模式生成的图形也是多边形
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

//选择并打开图层
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
/// 更新菜单状态
/// </summary>
void refreshMenuState()
{
	void checkMenuItem(int menuID, bool checked);
	void setMenuItemText(int menuID, const char* text);

	//修改菜单项文字
	//setMenuItemText(ID_ROAM_MODE, g_IsEditMode =="浏览模式（点击切到编辑模式）");

	//修改菜单项打勾状态
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

///处理窗口大小变化消息
void sized(int cx, int cy)
{
	if (cx == 0 || cy == 0)return;
	double aspectRatio = (double)cx / cy;
	boolpre();
	
	//如何根据投影矩阵直接得到远近平面参数
	//
	
	//g_Renderer.setViewport(0, 0, cx, cy);
}

///初始化
void initialize()
{
	g_pLayer = new Layer();
	g_Dataset.addLayer(g_pLayer);	

	g_OperationType = ot3DRotate;

	int width = getWindowWidth();
	int height = getWindowHeight();
	//g_Renderer.setWindow(0, w, 0, h);
	if (width == 0 || height == 0) return;
	//视口变换
	g_Renderer3D.g_viewport.makeScale(0.5, 0.5, 1.0);
	g_Renderer3D.g_viewport*= Matrix4::translate(0.5, 0.5, 0.0);
	////透视投影
	//boolpre();
	
	//g_Renderer3D.setViewport(width, height);
	
	//观察坐标系在世界坐标系中定义，构造观察坐标系
	
	Vec3 eye(600, 600, 600), center(0, 0, 0), up(0, 1, 0);
	g_Renderer3D.g_viewMatrix.makeLookAt(eye, center, up);
		
	//g_Renderer3D.lookAt(eye, center, up);
	
	//模型矩阵
	//假设使用的世界坐标系单位为像素，变换后绘制时y需要转换为视口像素坐标
	//g_pMesh = Box3D().toMesh();//模型，构造一个盒子，-1到1，再变换一下

	//g_modelMatrix.makeScale(100, 100, 100);
	//g_modelMatrix *= Matrix4::translate(300, 300, -100);
	
	g_Renderer3D.buildScene(&g_root);
	//g_Renderer3D.render(g_pMesh);
	refreshMenuState();
}
//投影方式
void boolpre()
{
	int width = getWindowWidth();
	int height = getWindowHeight();
	double aspectRatio = (double)width / height;
	if (!g_zj)
		g_Renderer3D.g_projectMatrix.makePerspective(g_fovy, aspectRatio, g_zNear, g_zFar);
	//正交投影
	else
		g_Renderer3D.g_projectMatrix.makeortho(700, 600, g_zNear, g_zFar);
	    //g_Renderer3D.g_projectMatrix.ortho(0,width,0,height,  g_zNear, g_zFar);
	
}


///销毁窗口时做清理工作
void destroy()
{
	//此处的代码直接写在退出按钮下了，直接采用init()初始化
}


#pragma endregion

#pragma region handleMessage

///所有消息的入口点
LRESULT handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND://菜单消息
	{
		int menuID = LOWORD(wParam);
		handleMenuMessage(menuID);
		return FALSE;
	}
	case WM_PAINT://绘制消息
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
	case WM_KEYDOWN://按键消息
	case WM_SYSKEYDOWN:
	{
		int key = wParam;//获得按键代码
		handleKeyMessage(key);
		return FALSE;
	}
	case WM_MOUSEMOVE://鼠标移动消息
	case WM_LBUTTONDOWN://鼠标左键按下消息	
	case WM_LBUTTONUP://鼠标左键弹起消息
	case WM_RBUTTONUP://鼠标右键弹起消息
	case WM_RBUTTONDOWN://鼠标右键按下消息
	case WM_LBUTTONDBLCLK://鼠标双击消息
	case WM_MOUSEWHEEL://鼠标滚轮消息
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		int det = message == WM_MOUSEWHEEL ? (short)HIWORD(wParam) : 0;
		handleMouseMessage(message, x, y, det);
		return FALSE;
	}
	case WM_SIZE://窗口大小变化消息
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





