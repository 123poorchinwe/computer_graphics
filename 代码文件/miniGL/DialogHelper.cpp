#include "DialogHelper.h"
#include <string>
#include <Windows.h>

//打开文件并选择
int DialogHelper::selectSingleFile(const char* intialDir,const char* extension, char* selectedFile , int bufSize )
{	
	std::string strFilter;
	strFilter.append("*.");
	strFilter.append( extension );
	strFilter.append("\0", 1 );
	strFilter.append("*.");
	strFilter.append( extension );
	strFilter.append("\0\0", 2 );

	char szBuffer[1024] = { 0 };	
	OPENFILENAMEA ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.nMaxFile = bufSize;
	ofn.lpstrFilter = strFilter.c_str();
	ofn.lpstrInitialDir = intialDir;//默认的文件路径
	ofn.lpstrFile = selectedFile;//存放文件的缓冲区
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;//标志如果是多选要加上OFN_ALLOWMULTISELECT
	return GetOpenFileNameA(&ofn) ;
}

extern HWND g_hWnd;
//选择颜色，后续进行背景和画笔颜色更改时调用此函数
bool DialogHelper::selectColor(Color& selectedColor , Color initialColor /*= BLACK */)
{
	CHOOSECOLOR stChooseColor;
	COLORREF acrCustClr[16];
	stChooseColor.lStructSize    = sizeof(CHOOSECOLOR) ;     
	stChooseColor.hwndOwner      = g_hWnd ;           
	stChooseColor.rgbResult      = initialColor ;       
	stChooseColor.lpCustColors   = acrCustClr;      
	stChooseColor.Flags          = CC_FULLOPEN |CC_RGBINIT;           
	stChooseColor.lCustData      = 0 ;            
	stChooseColor.lpfnHook       = NULL ;          
	stChooseColor.lpTemplateName = NULL ;

	if( ChooseColor(&stChooseColor))
	{
		selectedColor = stChooseColor.rgbResult;
		return true;
	}
	return false;
}
//更改字体
bool DialogHelper::selectFont(char* selectedFontName, int& selectedFontSize)
{
	CHOOSEFONTA cf;            // common dialog box structure
	LOGFONTA lf;			   // logical font structure
	strcpy_s( lf.lfFaceName , getFontName());
	lf.lfHeight =  fontSizeToFontHeight( getFontSize()); 

	// Initialize CHOOSEFONT
	ZeroMemory(&cf, sizeof(CHOOSEFONTA));
	cf.lStructSize = sizeof (CHOOSEFONTA);
	cf.lpLogFont = &lf;
	cf.Flags = CF_SCREENFONTS |CF_INITTOLOGFONTSTRUCT;

	if( ChooseFontA(&cf) ){
		selectedFontSize = fontHeightToFontSize( cf.lpLogFont->lfHeight );
		strcpy_s( selectedFontName , 32, cf.lpLogFont->lfFaceName );
		return true;
	}
	return false;
}
