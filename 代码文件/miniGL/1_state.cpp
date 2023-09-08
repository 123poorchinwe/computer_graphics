#include "1_State.h"

State g_State;

//初始化参数
State::State()
{
	DrawMode = dmPixel;

	GridColor1 = BLUE;
	GridColor2 = GREEN;
	GridWidth= GridHeight= 5;
	//确定格网间距为5
		
	CullMode= CULL_NONE;
	Fi11Mode= FILL_NONE;
}

