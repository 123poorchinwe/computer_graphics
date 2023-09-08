/********************************************************************
file base:	Graphic
file ext:	cpp
author:		胡斌(njnu2016@foxmail.com)
purpose:	图形库实现
version:    1.0
*********************************************************************/
#include "Graphic.h"
#include <windows.h>
#include <math.h>
#include <vector>
#include <assert.h>
#include <locale.h>
#include"1_Rasterprint.h"
using namespace std;

#pragma region DynamicMatrix
template< typename T>
class DynamicMatrix
{
public:
	DynamicMatrix(int alignBytes = 1) { this->alignBytes = alignBytes; pData = 0; width = height = 0; }
	~DynamicMatrix(void);

	void freeMatrix();
	T* operator[](int i);
	void setSize(int width, int height);
	T* dataPtr()const { return pData ? pData[0] : 0; }
	template<class U> void clear(const U& value);
	template<class U> void clear(int x, int y, int size_x, int size_y, const U& value);
	bool valid()const { return pData || width > 0 || height > 0; }
	int getLineWidth();

	int alignBytes;
	int width, height;
	T** pData;
};

template< class T>
DynamicMatrix<T>::~DynamicMatrix(void)
{
	freeMatrix();
}

template< typename T>
int DynamicMatrix<T>::getLineWidth()
{
	return (width * sizeof(T) + alignBytes - 1) / alignBytes * alignBytes;
}

template< typename T>
void DynamicMatrix<T>::setSize(int width, int height)
{
	if (width == this->width && height == this->height) return;

	this->width = width;
	this->height = height;

	//freeMatrix();	

	int lineWidth = getLineWidth();
	T* pBuf;
	if (pData)
	{
		pBuf = (T*)realloc(pData[0], lineWidth * height);
		pData = (T**)realloc(pData, sizeof(T*) * height);
	}
	else
	{
		//pBuf = (T*)malloc( sizeof(T) * width * height );
		pBuf = (T*)malloc(lineWidth * height);
		pData = (T**)malloc(sizeof(T*) * height);
	}

	for (int i = 0; i < height; ++i)
	{
		//pData[i] = pBuf + width * i;
		pData[i] = (T*)((char*)pBuf + lineWidth * i);
	}

}

template< typename T>
T* DynamicMatrix<T>::operator[](int i)
{
	return pData[i];
}

template< typename T>
void DynamicMatrix<T>::freeMatrix()
{
	if (pData)
	{
		free(pData[0]);
		free(pData);
		pData = 0;
	}
}

template< typename T >
template< typename U>
void DynamicMatrix<T>::clear(const U& value)
{
	if (!pData || !pData[0]) return;

	T* p = pData[0];
	int lineWidth = getLineWidth();
	if (sizeof(U) == 1)
	{
		memset(p, value, lineWidth * height);
		//memset( pData[0],value, sizeof(T) * width * height );
	}
	else
	{
		unsigned val = (unsigned&)value & 0xFF;
		if (val == (unsigned&)value)
		{
			memset(p, val, lineWidth * height);
		}
		else
		{
			int sizeT = sizeof(T);
			int sizeU = sizeof(U);

			if (sizeT <= sizeU)
			{
				for (int i = 0; i < height; ++i)
				{
					for (int j = 0; j < width; ++j)
					{
						unsigned char* pT = (unsigned char*)(pData[i] + j);
						unsigned char* pU = (unsigned char*)&value;
						for (int k = 0; k < sizeT; ++k)
						{
							pT[k] = pU[k];
						}
					}
					//p[i] = (T&)value;
				}
			}
			else
			{
				for (int i = 0; i < height; ++i)
				{
					for (int j = 0; j < width; ++j)
					{
						unsigned char* pT = (unsigned char*)(pData[i] + j);
						unsigned char* pU = (unsigned char*)&value;
						for (int k = 0; k < sizeU; ++k)
						{
							pT[k] = pU[k];
						}
					}

					//p[i] = (T&)value;
				}
			}
		}
	}
}

template<typename T>
template<typename U>
void DynamicMatrix<T>::clear(int x, int y, int size_x, int size_y, const U& value)
{
	if (!pData || !pData[0]) return;

	T* p = pData[0];
	int lineWidth = getLineWidth();
	int xend = x + size_x;
	if (sizeof(U) == 1)
	{
		for (int i = 0; i < size_y; ++i)
		{
			unsigned char* pT = (unsigned char*)(pData[y + i] + x);
			int size = xend < width ? size_x * sizeof(T) : lineWidth;
			memset(pT, value, size);
		}
	}
	else
	{
		unsigned val = (unsigned&)value & 0xFF;
		if (val == (unsigned&)value)
		{
			//memset( p,val, lineWidth * height );
			for (int i = 0; i < size_y; ++i)
			{
				unsigned char* pT = (unsigned char*)(pData[y + i] + x);
				int size = xend < width ? size_x * sizeof(T) : lineWidth;
				memset(pT, val, size);
			}
		}
		else
		{
			int sizeT = sizeof(T);
			int sizeU = sizeof(U);
			unsigned char* pU = (unsigned char*)&value;
			int leftBytes = xend < width ? 0 : lineWidth - xend * sizeof(T);

			if (sizeT <= sizeU)
			{

				for (int i = 0; i < size_y; ++i)
				{
					for (int j = 0; j < size_x; ++j)
					{
						unsigned char* pT = (unsigned char*)(pData[y + i] + j);
						for (int k = 0; k < sizeT; ++k)
						{
							pT[k] = pU[k];
						}
					}

					unsigned char* pT = xend * sizeof(T);
					for (int k = 0; k < leftBytes; ++k)
					{
						pT[k] = pU[k];
					}
				}
			}
			else
			{
				for (int i = 0; i < size_y; ++i)
				{
					for (int j = 0; j < size_x; ++j)
					{
						unsigned char* pT = (unsigned char*)(pData[y + i] + j);
						for (int k = 0; k < sizeU; ++k)
						{
							pT[k] = pU[k];
						}
					}

					unsigned char* pT = xend * sizeof(T);
					for (int k = 0; k < leftBytes; ++k)
					{
						pT[k] = pU[k];
					}

				}
			}
		}
	}
}

#pragma endregion DynamicMatrix

LRESULT CALLBACK WndProcNew(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
void setWindowSize(int width, int height);
WNDPROC g_lpfnOldProc = 0;

int g_clientWidth = 800;
int g_clientHeight = 600;

POINT g_orig = { 0, 0 };
int g_upY = 1;

HWND g_hWnd = 0;
HDC g_hDC = 0;
HFONT g_hOldFont = 0;

unsigned g_backColor =  WHITE;
float g_fontSize = 28;
char g_fontName[32] = "宋体";

//int g_pointSize = 10;
unsigned g_penColor = 0;
bool g_isDragging = false;
POINT g_lastPoint = { 0, 0 };



#ifndef DIRECT_DRAW

HDC g_hdcMem = 0;
typedef unsigned char rgb[3];
DynamicMatrix<rgb> g_frameBuffer(4);
DynamicMatrix<byte> g_frameBuffer_Alpha;
DynamicMatrix<float> g_zBuffer;
DynamicMatrix<unsigned> g_stencilBuffer;

BITMAPINFO g_bmpInfo;
HBITMAP g_hBitmap;

struct DummyClass
{
	DummyClass()
	{

	}
	~DummyClass()
	{ 
		if ( g_hdcMem )	DeleteObject(g_hdcMem);
	}
} g_dummyClass;

#endif

BOOL CALLBACK _enumWindowCallBack(HWND hWnd, LPARAM lParam)
{
	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	if (dwProcessId == *(DWORD*)lParam)
	{
		g_hWnd = hWnd;
		return FALSE;
	}

	return TRUE;
}

BOOL _isConsoleApplication()
{
	return GetConsoleWindow() != NULL;
}

void _changeFont();
void applyPenColor();
int init(unsigned hwnd)
{
	setlocale( LC_ALL, "");

	g_hWnd = (HWND)hwnd;

	//g_lpfnOldProc = (WNDPROC)SetWindowLong(g_hWnd, GWL_WNDPROC, (DWORD)WndProcNew);
	g_lpfnOldProc = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProcNew);
	g_hDC = GetDC(g_hWnd);

	if (g_hDC)
	{
#ifndef DIRECT_DRAW

		g_hdcMem = CreateCompatibleDC(g_hDC);
		g_bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		g_bmpInfo.bmiHeader.biPlanes = 1;
		g_bmpInfo.bmiHeader.biBitCount = 24;
		g_bmpInfo.bmiHeader.biCompression = BI_RGB;
		g_bmpInfo.bmiHeader.biSizeImage = 0;
		g_bmpInfo.bmiHeader.biXPelsPerMeter = 3000;
		g_bmpInfo.bmiHeader.biYPelsPerMeter = 3000;
		g_bmpInfo.bmiHeader.biClrUsed = 0;
		g_bmpInfo.bmiHeader.biClrImportant = 0;
#endif
		applyPenColor();

		RECT rect;
		GetClientRect(g_hWnd, &rect);

		setWindowSize(rect.right, rect.bottom );

		setYUp( g_upY == -1 ? true : false );
		setOrig( g_orig.x, g_orig.y );

		_changeFont();

		return 0;
	}
	return -1;
}

int init()
{
	DWORD hProcessID = GetCurrentProcessId();

	// 查找主窗口  
	EnumWindows(_enumWindowCallBack, (LPARAM)&hProcessID);

	return init( (unsigned)g_hWnd);
}

void _ensure_inited()
{
	if (!g_hDC)	init( );
	if (!g_hDC)exit(-1);
}

#ifndef DIRECT_DRAW

void _initMemDC()
{
	g_hBitmap = CreateCompatibleBitmap(g_hDC, g_clientWidth, g_clientHeight);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(g_hdcMem, g_hBitmap);
	DeleteObject(hOldBitmap);
}

#endif

void setWindowSize(int width, int height)
{

	if (!g_hDC)
	{
		init();
		return;
	}	

	if (_isConsoleApplication())
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_FONT_INFO fontInfo;
		GetCurrentConsoleFont(handle, TRUE, &fontInfo);
		COORD fontSize = GetConsoleFontSize(handle, fontInfo.nFont);

		double scaleX = GetDeviceCaps(g_hDC, LOGPIXELSX) / 72.0;
		double scaleY = GetDeviceCaps(g_hDC, LOGPIXELSY) / 72.0;
		fontSize.X *= scaleX;//font pixel num
		fontSize.Y *= scaleY;//font pixel num

		COORD size;
		if (width == -1 && height == -1)
		{
			size = GetLargestConsoleWindowSize(handle);
		}
		else
		{		
			size.X = (double)width / fontSize.X;
			size.Y = (double)height / fontSize.Y;			

			COORD sizeMax = GetLargestConsoleWindowSize(handle);
			size.X = size.X > sizeMax.X ? sizeMax.X : size.X;
			size.Y = size.Y > sizeMax.Y ? sizeMax.Y : size.Y;
		}

		int rs = SetConsoleScreenBufferSize(handle, size);

		SMALL_RECT rc;
		rc.Left = 0, rc.Top = 0, rc.Right = size.X - 1, rc.Bottom = size.Y - 1;
		rs = SetConsoleWindowInfo(handle, TRUE, &rc);

		CONSOLE_SCREEN_BUFFER_INFO bInfo;
		GetConsoleScreenBufferInfo(handle, &bInfo);

		g_clientWidth = (bInfo.srWindow.Right - bInfo.srWindow.Left) * fontSize.X;
		g_clientHeight = (bInfo.srWindow.Bottom - bInfo.srWindow.Top) * fontSize.Y;
	}
	else
	{
		//if (width != -1 && height != -1)
		//{
		//	RECT wndRect = { 0, 0, width, height };// client Rect
		//	AdjustWindowRectEx(&wndRect, WS_OVERLAPPEDWINDOW, FALSE, NULL);
		//	MoveWindow(g_hWnd, 0, 0, wndRect.right, wndRect.bottom, TRUE);
		//}	
		RECT rect;
		GetClientRect(g_hWnd, &rect);
		g_clientWidth = rect.right - rect.left;
		g_clientHeight = rect.bottom - rect.top;
	}	

#ifndef DIRECT_DRAW
	_initMemDC();
	g_frameBuffer.setSize(g_clientWidth, g_clientHeight);
	g_frameBuffer_Alpha.setSize(g_clientWidth, g_clientHeight);
	g_zBuffer.setSize( g_clientWidth, g_clientHeight);
	g_stencilBuffer.setSize( g_clientWidth, g_clientHeight);

	g_bmpInfo.bmiHeader.biWidth = g_clientWidth;
	g_bmpInfo.bmiHeader.biHeight = -g_clientHeight;
	//g_frameBuffer.clearWithValue(g_backColor);
#endif
}

#ifndef DIRECT_DRAW

void _buildMemDCFromFrameBuffer( )
{
	SetDIBits( g_hdcMem, g_hBitmap, 0, abs( g_bmpInfo.bmiHeader.biHeight), g_frameBuffer.dataPtr(), &g_bmpInfo, DIB_RGB_COLORS); 

	//for (int row = 0; row < g_frameBuffer.height; ++row)
	//{
	//	for (int col = 0; col < g_frameBuffer.width; ++col)
	//	{
	//		unsigned color = g_frameBuffer[row][col];
	//		if (color != g_backColor)
	//			::SetPixel(g_hdcMem, col, row, color);
	//	}
	//}
}


void _clearMemDC()
{
	static HBRUSH hBrush = CreateSolidBrush( g_backColor );
	RECT rect;
	GetClientRect( g_hWnd, &rect);
	::FillRect(g_hdcMem, &rect , hBrush);
}

/**	把后缓存信息显示到屏幕
*/
void swapBuffer()
{
	_ensure_inited();

	_buildMemDCFromFrameBuffer();

	RECT rect;
	GetClientRect(g_hWnd, &rect);

	BitBlt(g_hDC, 0, 0, g_clientWidth, g_clientHeight /*g_frameBuffer.width, g_frameBuffer.height*/, g_hdcMem, 0, 0, SRCCOPY);
}


#endif

void refreshWindow()
{
	InvalidateRect(g_hWnd, NULL, TRUE);
}

unsigned RGBtoBGR( unsigned color )
{
	unsigned char* pColor = (unsigned char*)&color;
	unsigned char c = pColor[0] ;
	pColor[0] = pColor[2];
	pColor[2] = c;
	return color;
}

void clearWindow()
{
	_ensure_inited();	

#ifndef DIRECT_DRAW
	g_frameBuffer.clear( RGBtoBGR( g_backColor));
	g_frameBuffer_Alpha.clear(255);
	g_zBuffer.clear( (float)1.0 );
	//_buildMemDCFromFrameBuffer();

	_clearMemDC();
#else
	static HBRUSH hBrush = CreateSolidBrush( g_backColor );
	RECT rect;
	GetClientRect( g_hWnd, &rect);
	DPtToLPt( rect.left, rect.top, (int&)rect.left, (int&)rect.top );
	DPtToLPt( rect.right, rect.bottom, (int&)rect.right, (int&)rect.bottom );
	::FillRect(g_hDC, &rect , hBrush);
#endif	
}

void setOrig(int x, int y)
{
	_ensure_inited();

#ifdef DIRECT_DRAW
	//SetWindowOrgEx(g_hDC, -x, getWindowHeight() - y, 0);
	//SetWindowOrgEx(g_hdcMem, -x, getWindowHeight() - y, 0);
	SetViewportOrgEx(g_hDC, x, y, 0);
	//SetViewportOrgEx(g_hdcMem, x, y, 0);
#endif
	g_orig.x = x;
	g_orig.y = y;

}

void getOrig(int& x, int& y)
{
	_ensure_inited();

	x = g_orig.x;
	y = g_orig.y;
}

void setYUp(bool isUp)
{
#ifdef DIRECT_DRAW
	if (isUp)
	{
		SetMapMode(g_hDC, MM_ANISOTROPIC);
		SetWindowExtEx(g_hDC, 1, 1, 0);
		SetViewportExtEx(g_hDC, 1, -1, 0);

		//SetMapMode(g_hdcMem, MM_ANISOTROPIC);
		//SetWindowExtEx(g_hdcMem, 1, 1, 0);
		//SetViewportExtEx(g_hdcMem, 1, -1, 0);
	}
	else
	{
		SetMapMode(g_hDC, MM_ANISOTROPIC);
		SetWindowExtEx(g_hDC, 1, 1, 0);
		SetViewportExtEx(g_hDC, 1, 1, 0);

		//SetMapMode(g_hdcMem, MM_ANISOTROPIC);
		//SetWindowExtEx(g_hdcMem, 1, 1, 0);
		//SetViewportExtEx(g_hdcMem, 1, 1, 0);
	}
#endif
	g_upY = isUp ? -1 : 1;

}

bool isYUp()
{
	return g_upY == -1;
}

/**	设置背景色
@param  backColor 背景色
*/
void setBackColor( Color color )
{
	g_backColor = color;
}

Color getPenColor()
{
	return g_penColor;
}

void setPenColor(Color color)
{
	if (g_penColor == color) return;

	g_penColor = color;

	applyPenColor();
}

void applyPenColor()
{
#ifdef DIRECT_DRAW
	SelectObject(g_hDC, GetStockObject(DC_PEN));
	SetDCPenColor(g_hDC, g_penColor);
	SelectObject(g_hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(g_hDC, g_penColor);
#endif // DIRECT_DRAW
}

double getPixelSize()
{
	HDC hdc = GetDC(NULL);
	double s = (double)(hdc , HORZSIZE) / GetSystemMetrics(SM_CXSCREEN);
	ReleaseDC(NULL, hdc);
	return s;
}

void _changeFont()
{
	int lfHeight =  fontSizeToFontHeight( g_fontSize ); 
	HFONT hFont = CreateFontA( lfHeight, 0, 0/*g_fontDirection*10*/, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, g_fontName );
	if( g_hOldFont )
	{
		DeleteObject( g_hOldFont );
	}
	g_hOldFont = (HFONT)SelectObject( g_hDC, hFont );
}

int getFontSize()
{
	return g_fontSize;
}

void setFontSize( int fontSize )
{
	if( g_fontSize != fontSize )
	{
		g_fontSize = fontSize;
		_changeFont();
	}

}

int fontSizeToFontHeight(int fontSize)
{
	return MulDiv( fontSize,GetDeviceCaps(GetDC(NULL),LOGPIXELSX),72); 
}

int fontHeightToFontSize(int fontHeight)
{
	return MulDiv( abs(fontHeight) , 72 , GetDeviceCaps(GetDC(NULL),LOGPIXELSX) );
}

const char* getFontName()
{
	return g_fontName;
}

void setFontName( const char* fontName )
{
	if( _stricmp( g_fontName, fontName ) )
	{
		strcpy_s( g_fontName, fontName );
		_changeFont();
	}

}

void drawLine(int x0, int y0, int x1, int y1)
{
#ifdef DIRECT_DRAW
	_ensure_inited();
	::MoveToEx(g_hDC, x0, y0, 0);
	::LineTo(g_hDC, x1, y1);
#endif
}

void drawPolygon(PixelPoint* pts, int count)
{
#ifdef DIRECT_DRAW
	_ensure_inited();

	vector<POINT> _pts(count);
	for ( int i = 0; i < count; ++i )
	{
		_pts[i].x = pts[i].x;
		_pts[i].y = pts[i].y;
	}

	Polygon(g_hDC, _pts.data(), count);
#endif
}


void _setPixel(int x, int y, Color color)
{
	//_ensure_inited();

#ifndef DIRECT_DRAW

	if (x < 0 || x >= g_frameBuffer.width) return;
	if (y < 0 || y >= g_frameBuffer.height) return;

	char* pBytes = (char*)( g_frameBuffer[y] + x);
	char* pColor = (char*)&color;
	pBytes[0] = pColor[2];
	pBytes[1] = pColor[1];
	pBytes[2] = pColor[0];
	g_frameBuffer_Alpha[y][x] = pColor[3];
	//g_frameBuffer[y][x] = color;
#else
	//SetPixel( g_hdcMem, x, y, color );
	SetPixel(g_hDC, x, y, color);
#endif
}

void _setPixel(int x, int y, float z, Color color)
{
#ifndef DIRECT_DRAW
	if (x < 0 || x >= g_frameBuffer.width) return;
	if (y < 0 || y >= g_frameBuffer.height) return;
	if( z < 0 || z > 1.0 || z >= g_zBuffer[y][x]) return ;

	char* pBytes = (char*)( g_frameBuffer[y] + x);
	char* pColor = (char*)&color;
	pBytes[0] = pColor[2];
	pBytes[1] = pColor[1];
	pBytes[2] = pColor[0];
	g_zBuffer[y][x] = z;
	g_frameBuffer_Alpha[y][x] = pColor[3];
	//g_frameBuffer[y][x] = color;
#else
	//SetPixel( g_hdcMem, x, y, color );
	SetPixel(g_hDC, x, y, color);
#endif
}

void setPixel(int x, int y, Color color)
{
#ifndef DIRECT_DRAW
	LPtToDPt( x,y, x,y);
#endif
	_setPixel( x, y, color );
}

void setPixel(int x, int y, float z, Color color)
{
#ifndef DIRECT_DRAW
	LPtToDPt( x,y, x,y);
#endif
	_setPixel( x, y, z , color );
}

/**	设置指定物理位置像素的颜色，坐标系原点位于左上角，x向右，y向下
@param  x 像素x坐标
@param  y  像素y坐标
@param color 颜色
*/
void setDevicePixel(int x, int y, Color color)
{
#ifdef DIRECT_DRAW
	POINT pt = { x, y };
	::DPtoLP( g_hDC, &pt, 1 );
	x = pt.x, y = pt.y;
#endif
	_setPixel( x, y, color );
}

/**	设置指定物理位置像素的颜色，坐标系原点位于左上角，x向右，y向下
@param  x 像素x坐标
@param  y  像素y坐标
@param color 颜色
*/
void setDevicePixel(int x, int y, float z, Color color)
{
#ifdef DIRECT_DRAW
	POINT pt = { x, y };
	::DPtoLP( g_hDC, &pt, 1 );
	x = pt.x, y = pt.y;
#endif
	_setPixel( x, y, z, color );
}

Color getDevicePixel(int x, int y)
{
#ifndef DIRECT_DRAW

	if (x < 0 || x >= g_frameBuffer.width) return 0;
	if (y < 0 || y >= g_frameBuffer.height) return 0;

	char* pBytes = (char*)( g_frameBuffer[y] + x);
	unsigned color = 0;
	char* pColor = (char*)&color;
	pColor[0] = pBytes[2];
	pColor[1] = pBytes[1];
	pColor[2] = pBytes[0];
	pColor[3] = g_frameBuffer_Alpha[y][x];
	return color;
	//return g_frameBuffer[y][x];
#else
	//return GetPixel( g_hdcMem, x, y );
	return GetPixel(g_hDC, x, y);
#endif
}

Color getPixel(int x, int y)
{
	//_ensure_inited();

#ifndef DIRECT_DRAW
	LPtToDPt( x,y, x,y);
#endif
	return getDevicePixel( x,  y);
}

void DPtToLPt(int dx, int dy, int& lx, int& ly)
{
#ifndef DIRECT_DRAW
	lx = dx - g_orig.x;
	ly = (dy - g_orig.y)*g_upY;
#else
	//lx = dx;
	//ly = dy;
	POINT pt = { dx, dy };
	DPtoLP( g_hDC, &pt, 1 );
	lx = pt.x;// dx;
	ly = pt.y;// dy;
#endif
}

void LPtToDPt( int lx, int ly, int& dx, int& dy )
{
#ifndef DIRECT_DRAW
	dx = lx + g_orig.x;
	dy = g_orig.y + g_upY * ly;
#else
	//dx = lx;
	//dy = ly;
	POINT pt = { lx, ly };
	LPtoDP( g_hDC, &pt, 1 );
	dx = pt.x;
	dy = pt.y;
#endif
}

int getWindowWidth()
{
	return g_clientWidth;
}

int getWindowHeight()
{
	return g_clientHeight;
}


bool isShiftKeyPressed()
{
	return GetKeyState(VK_SHIFT) < 0;
}

bool isCtrlKeyPressed()
{
	return GetKeyState(VK_CONTROL) < 0;
}

bool isAltKeyPressed()
{
	return GetKeyState(VK_MENU) < 0;
}

Color getBackColor()
{
	return g_backColor;
}

class RubberPad
{
public:
	POINT pt1;
	POINT pt2;
	vector<POINT> pts;
	Color penColor;
	Color oldPenColor;
	int oldROP2Mode;
	RubberMode rubberMode ;
	bool startDrawing ;
	bool lineToFirst;

	RubberPad()
	{
		penColor = YELLOW;
		rubberMode = rmNone;
		startDrawing = false;
		lineToFirst = true;
	}

	void mouseDown(UINT wMsg, WPARAM wParam, LPARAM lParam)
	{
	
		
	}

	void mouseMove(UINT wMsg, WPARAM wParam, LPARAM lParam)
	{
		if (rubberMode == rmNone) return;

		if (startDrawing)
		{
			drawRubberline(g_hDC, pt1.x, pt1.y, pt2.x, pt2.y);

			if( rubberMode == rmPolygon && pts.size() >= 2 )
			{
				if (lineToFirst)
					drawRubberline(g_hDC, pts[0].x, pts[0].y, pt2.x, pt2.y);
				else
					lineToFirst = true;
			}			

			pt2.x = LOWORD(lParam);
			pt2.y = HIWORD(lParam);

			drawRubberline(g_hDC, pt1.x, pt1.y, pt2.x, pt2.y);

			if( rubberMode == rmPolygon && pts.size() >= 2 )
			{
				drawRubberline(g_hDC, pts[0].x, pts[0].y, pt2.x, pt2.y);
			}
		}

	}

	void mouseUp(UINT wMsg, WPARAM wParam, LPARAM lParam)
	{
		if (wMsg == WM_LBUTTONUP)
		{
			if (rubberMode == rmNone) return;

			if( !startDrawing )
			{
				oldROP2Mode = SetROP2(g_hDC, R2_NOT);
				SelectObject(g_hDC, GetStockObject(DC_PEN));
				oldPenColor = SetDCPenColor(g_hDC, penColor);

				pt1.x = LOWORD(lParam);
				pt1.y = HIWORD(lParam);

				pt2 = pt1;
				pts.clear();

				startDrawing = true;
			}
			else
			{
				if( rubberMode == rmLine || rubberMode == rmRectangle )
				{
					drawRubberline(g_hDC, pt1.x, pt1.y, pt2.x, pt2.y);	

					pts.push_back( pt2 );

					DPtToLPt( pts[0].x, pts[0].y, (int&)pts[0].x, (int&)pts[0].y );
					DPtToLPt( pts[1].x, pts[1].y, (int&)pts[1].x, (int&)pts[1].y );
					//DPtToLPt( pt1.x, pt1.y, (int&)pt1.x, (int&)pt1.y );
					//DPtToLPt( pt2.x, pt2.y, (int&)pt2.x, (int&)pt2.y );

					SelectObject(g_hDC, GetStockObject(DC_PEN));
					SetDCPenColor(g_hDC, oldPenColor);
					SetROP2(g_hDC, oldROP2Mode );
					startDrawing = false;

					return;
				}
				else
				{
					if( rubberMode == rmPolygon && pts.size() == 1 )
					{
						drawRubberline(g_hDC, pt1.x, pt1.y, pt2.x, pt2.y);//擦除
						lineToFirst = false;//多边形模式下防止第一条线被擦除
					}

					pt2.x = LOWORD(lParam);
					pt2.y = HIWORD(lParam);

					if( rubberMode == rmPolygon && pts.size() == 1 )
					{
						drawRubberline(g_hDC, pt1.x, pt1.y, pt2.x, pt2.y);
					}				

					pt1 = pt2;
				}				
			}

			pts.push_back( pt1 );

		}
		else if( wMsg == WM_RBUTTONUP )
		{
			if( startDrawing == false ) return;

			if( rubberMode == rmPolyline || rubberMode == rmPolygon )
			{
				drawRubberline(g_hDC, pt1.x, pt1.y, pt2.x, pt2.y);	//擦除P1P2

				if( rubberMode == rmPolygon && pts.size() >= 2 ) // >= 2 --> P2 与两个点相连
				{
					if (lineToFirst)
						drawRubberline(g_hDC, pts[0].x, pts[0].y, pt2.x, pt2.y);
					else
						lineToFirst = true;	 //擦除P0P2
				}

				for ( int i = 0 ; i < (int)pts.size() - 1; ++i )
				{
					drawRubberline(g_hDC, pts[i].x, pts[i].y, pts[i+1].x, pts[i+1].y);
				}

				for ( int i = 0 ; i < (int)pts.size() ; ++i )
				{
					DPtToLPt( pts[i].x, pts[i].y, (int&)pts[i].x, (int&)pts[i].y );
				}

				SelectObject(g_hDC, GetStockObject(DC_PEN));
				SetDCPenColor(g_hDC, oldPenColor);
				SetROP2(g_hDC, oldROP2Mode );
				startDrawing = false;
			}
		}
	}

	void drawRubberline(HDC hdc, int x0, int y0, int x1, int y1)
	{

		if (rubberMode != rmRectangle )
		{
			drawLine(hdc, x0, y0, x1, y1);
			//g_Raster.DDA_Line(x0, y0, x1, y1, getPenColor());
		}
		else
		{
			drawRectangle(hdc, x0, y0, x1, y1);
		}
	}

	void drawLine(HDC hdc, int x0, int y0, int x1, int y1)
	{
#ifdef DIRECT_DRAW
		DPtToLPt( x0, y0, x0 ,y0 );
		DPtToLPt( x1, y1, x1 ,y1 );
#endif
		MoveToEx(hdc, x0, y0, NULL);
		LineTo(hdc, x1, y1);
	}

	void drawRectangle(HDC hdc, int x0, int y0, int x1, int y1)
	{
#ifdef DIRECT_DRAW
		DPtToLPt( x0, y0, x0 ,y0 );
		DPtToLPt( x1, y1, x1 ,y1 );
#endif
		MoveToEx(hdc, x0, y0, NULL);
		LineTo(hdc, x1, y0);
		LineTo(hdc, x1, y1);
		LineTo(hdc, x0, y1);
		LineTo(hdc, x0, y0);
	}
};

RubberPad g_rubberPad;

void setRubberMode(RubberMode mode)
{
	g_rubberPad.pts.clear();
	g_rubberPad.rubberMode = mode;
}

RubberMode getRubberMode()
{
	return g_rubberPad.rubberMode;
}

int getRubberPointCount()
{
	return g_rubberPad.pts.size();
}

int getRubberPoints(int& x1, int& y1, int& x2, int& y2)
{
	int size = g_rubberPad.pts.size();
	if (size > 0)
	{
		x1 = g_rubberPad.pts[0].x;
		y1 = g_rubberPad.pts[0].y;
	}
	if (size > 1)
	{
		x2 = g_rubberPad.pts[1].x;
		y2 = g_rubberPad.pts[1].y;
	}
	return size < 3 ? size : 2;

}

int getRubberPoints(PixelPoint& pt1, PixelPoint& pt2)
{
	return getRubberPoints( pt1.x, pt1.y, pt2.x, pt2.y );
}

int getRubberPoints( PixelPoint pts[] )
{
	switch( g_rubberPad.rubberMode )
	{
	case rmLine:
	case rmRectangle:
		
	case rmPolyline:
	case rmPolygon:
		for( int i = 0 ; i < g_rubberPad.pts.size(); ++i )
		{
			pts[i].x = g_rubberPad.pts[i].x;
			pts[i].y = g_rubberPad.pts[i].y;
		}
		return g_rubberPad.pts.size();
	}
	return 0;
}

void setCursor(CursorStyle cursor)
{
	SetClassLong(g_hWnd, GCLP_HCURSOR, (long)LoadCursor(NULL, MAKEINTRESOURCE( cursor )));
}


byte* getWCharGlyph(wchar_t ch , int& offset_x, int& offset_y, int& gryph_width, int& gryph_height, int& cell_width, int& cell_height)
{
	

	GLYPHMETRICS gm;
	TEXTMETRIC tm;
	GetTextMetrics( g_hDC, &tm);

	MAT2 mat2 = {{0,1}, {0,0}, {0,0}, {0,1}};

	//获取这个字符图形需要的字节的大小。
	DWORD dwNeedSize = GetGlyphOutlineW(g_hDC,ch,GGO_BITMAP,&gm,0,NULL,&mat2);
	assert( dwNeedSize < 0xFFFF);

	static BYTE buf[0xFFFF];

	GetGlyphOutlineW( g_hDC,ch,GGO_BITMAP,&gm,dwNeedSize,buf,&mat2);

	offset_x = gm.gmptGlyphOrigin.x;
	offset_y = tm.tmAscent  - gm.gmptGlyphOrigin.y;//gm.gmptGlyphOrigin.y相对于baseline
	cell_width = gm.gmCellIncX;
	cell_height = tm.tmAscent + tm.tmDescent;// gm.gmCellIncY;
	gryph_width = ((gm.gmBlackBoxX + 31) >> 5) << 5;//DWORD对齐
	gryph_height = gm.gmBlackBoxY;	

	//SelectObject(g_hDC,hOldFont);
	//DeleteObject(hFont);

	return dwNeedSize ? buf : 0;
}

byte* getCharGlyph(  char ch , int& offset_x, int& offset_y, 
	int& gryph_width, int& gryph_height, int& cell_width, int& cell_height )
{
	//setlocale( LC_ALL, "");

	char cha[2] = {0};
	cha[0] = ch;

	wchar_t chw[2] = {0};

	size_t convertedChars = 0;
	mbstowcs_s( &convertedChars , chw, cha, 1);

	if (convertedChars == 0) return 0;

	return getWCharGlyph( chw[0] , offset_x, offset_y, gryph_width, gryph_height, cell_width, cell_height );

}

byte getCharGlyphPixel(byte * pCharGlyph, int x, int y, int offset_x, int offset_y, int gryph_width, int gryph_height)
{
	if( pCharGlyph == NULL ) return 0;
	if( x < offset_x || y < offset_y || x >= offset_x + gryph_width || y >= offset_y + gryph_height ) return 0;

	int gryph_width_byte = gryph_width >> 3;//每行字节数
	pCharGlyph += (y - offset_y) * gryph_width_byte ;//定位到该行
	x -=  offset_x;//pBuf只记录以（offset_x, offset_y）为左上角的图像信息
	pCharGlyph += x >> 3;//定位到列所在字节
	byte btCode = *pCharGlyph;
	x -= (x >> 3) << 3;//定位到列:减去整字节数*8
	return (btCode & ( 0x80 >> x )) ? 1 : 0;//该位是否为1？
}

LRESULT CALLBACK WndProcNew(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wMsg) 
	{
	case WM_SIZE:
		setWindowSize(-1, -1);
		CallWindowProc(g_lpfnOldProc, hWnd, wMsg, wParam, lParam);
		refreshWindow();
		return 0;
	case WM_ERASEBKGND:
#ifndef DIRECT_DRAW
		//clearWindow();			
		return TRUE;
#endif
		break;
	case WM_PAINT:
		if (g_rubberPad.startDrawing) return 0;

		clearWindow();

		CallWindowProc(g_lpfnOldProc, hWnd, wMsg, wParam, lParam);
		ValidateRect( g_hWnd, NULL );

#ifndef DIRECT_DRAW			
		swapBuffer();
#endif
		return 0;
	case WM_LBUTTONDOWN:
		CallWindowProc(g_lpfnOldProc, hWnd, wMsg, wParam, lParam);
		g_rubberPad.mouseDown(wMsg, wParam, lParam);
		return 0;
	case WM_MOUSEMOVE:
		g_rubberPad.mouseMove(wMsg, wParam, lParam);
		CallWindowProc(g_lpfnOldProc, hWnd, wMsg, wParam, lParam);
		return 0;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		g_rubberPad.mouseUp(wMsg, wParam, lParam);
		CallWindowProc(g_lpfnOldProc, hWnd, wMsg, wParam, lParam);
		return 0;
	case WM_MOUSEWHEEL:
		{
			POINT pt= {LOWORD(lParam),HIWORD( lParam)};
			::ScreenToClient( hWnd, &pt );//屏幕坐标转为窗口客户区坐标
			CallWindowProc(g_lpfnOldProc, hWnd, wMsg, wParam, MAKELPARAM( (short)pt.x, (short)pt.y ));
		}
		return 0;

	}
	return CallWindowProc(g_lpfnOldProc, hWnd, wMsg, wParam, lParam);
}


