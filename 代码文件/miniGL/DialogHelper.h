#pragma once

#include "Graphic.h"

class DialogHelper
{
public:
	/**	打开文件对话框，选择单个文件
	@param  intialDir 初始目录，可为0或""
	@param  extension  文件后缀
	@param  selectedFile  选择的文件
	@param  bufSize  selectedFile数组大小
	@return 如果用户取消或关闭Open对话框或错误出现，返回值为0，否则非0
	*/
	static int selectSingleFile( const char* intialDir,const char* extension, char* selectedFile, int bufSize );

	/**	打开颜色选择对话框，选择颜色
	@param  selectedColor 用户选择的颜色
	@param  initialColor  初始颜色
	@return 如果用户取消，返回值为false，否则为true
	*/
	static bool selectColor( Color& selectedColor , Color initialColor = BLACK );

	/**	打开字体选择对话框，选择字体
	@param  selectedFontName 用户选择的字体名字
	@param  selectedFontSize  用户选择的字体大小
	@return 如果用户取消，返回值为false，否则为true
	*/
	static bool selectFont( char* selectedFontName, int& selectedFontSize );
};

