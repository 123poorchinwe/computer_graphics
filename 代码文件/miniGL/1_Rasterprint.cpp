#include"1_Rasterprint.h"
#include <stdlib.h>
#include"Graphic.h"
#include"1_Grid.h"
#include "GeoDefine.h"
#include"1_state.h"

#include <stack>
Raster g_Raster;

//DDA算法
void Raster::DDA_Line(int x1, int y1, int x2, int y2,Color penc)
{
	float increx, increy, x, y, length;//变量定义
	int i;
	if (abs(x2 - x1) > abs(y2 - y1))//判断以哪个作增量
		length = abs(x2 - x1);
	else
		length = abs(y2 - y1);
	increx = (x2 - x1) / length;//设置增量，一个为1，一个为k
	increy = (y2 - y1) / length;
	x = x1, y = y1;//起点
	for (i = 1; i <= length; i++)
    {
        //网格绘制线
		if (g_State.DrawMode != dmPixel)
		{
            int xx = int(x + 0.5);
            int yy = int(y + 0.5);
			g_Grid.pixelToGrid(xx, yy, xx, yy);//每个像素转为对应行列号
            g_Grid.drawCell(xx, yy, BLACK);
			x += increx;//x+增量
			y += increy;//y+增量
		}
        else 
        {
            setPixel(int(x + 0.5), int(y + 0.5), penc);//因为没有半个像素点，所以需要强制转换为整型
            x += increx;//x+增量
            y += increy;//y+增量

        }
	}
}

//bresenham
void Raster::bresenham(int x0, int y0, int x1, int y1, Color penc)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int erro = (dx > dy ? dx : -dy) / 2;

	while (setPixel(x0, y0, penc), x0 != x1 || y0 != y1)
    {
		int e2 = erro;
		if (e2 > -dx) { erro -= dy; x0 += sx; }
		if (e2 < dy) { erro += dx; y0 += sy; }
	}
}


//画像素多边形,每条边使用光栅化方式逐像素绘制
void Raster::drawPolygonline(PixelPoint* pts, int count,Color color)
{
	//vector<POINT> _pts(count);
	for (int i = 0; i < count-1; ++i)
	{
		/*_pts[i].x = pts[i].x;
		_pts[i].y = pts[i].y;*/
		if (g_State.DrawLineMethod == dlDDA)
			//drawLine(v0.x(), v0.y(), v1.x(), v1.y());
			g_Raster.DDA_Line(pts[i].x, pts[i].y, pts[i + 1].x, pts[i + 1].y, color);
		else if (g_State.DrawLineMethod == dlMidBresenham)
			g_Raster.bresenham(pts[i].x, pts[i].y, pts[i + 1].x, pts[i + 1].y, getPenColor());
		
	}
	if (g_State.DrawLineMethod == dlDDA)
		//drawLine(v0.x(), v0.y(), v1.x(), v1.y());
		g_Raster.DDA_Line(pts[count - 1].x, pts[count - 1].y, pts[0].x, pts[0].y, color);
	else if (g_State.DrawLineMethod == dlMidBresenham)
		g_Raster.bresenham(pts[count - 1].x, pts[count - 1].y, pts[0].x, pts[0].y, getPenColor());
	
}

//X扫描线填充多边形
//定义用于边表ET和活动边表AET的通用类Edge,z和x通用
class Edge
{
public:
    int ymax;
    float x;
    float dx;
    float z;
    float dz;
    Edge* next;
};

//多边形X扫描线活性边表法填充
void Raster::polygonScan(PixelPoint* pts, int count, Color penc)
{
    //计算最高点的y坐标
    int maxY = 0;
    for (int i = 0; i < count; i++)
    {
        if (pts[i].y > maxY)
        {
            maxY = pts[i].y;
        }
    }
    //初始化边表ET和活动边表AET
    
    Edge* ET[1800];
    for (int i = 0; i < maxY; i++)
    {
        ET[i] = new Edge();
        ET[i]->next = nullptr;
    }
    Edge* AET = new Edge();
    AET->next = nullptr;

    //建立边表ET
    for (int i = 0; i < count; i++)
    {
        //取出当前点1前后相邻的共4个点，点1与点2的连线作为本次循环处理的边，另外两个点点0和点3用于计算奇点
        int x0 = pts[(i - 1 + count) % count].x;
        int x1 = pts[i].x;
        int x2 = pts[(i + 1) % count].x;
        int x3 = pts[(i + 2) % count].x;
        int y0 = pts[(i - 1 + count) % count].y;
        int y1 = pts[i].y;
        int y2 = pts[(i + 1) % count].y;
        int y3 = pts[(i + 2) % count].y;

       
        //水平线直接舍弃
        if (y1 == y2)
            continue;
        //分别计算下端点y坐标、上端点y坐标、下端点x坐标和斜率倒数
        int ymin = y1 > y2 ? y2 : y1;
        int ymax = y1 > y2 ? y1 : y2;
        float x = y1 > y2 ? x2 : x1;
        float dx = (x1 - x2) * 1.0f / (y1 - y2);
        //奇点特殊处理，若点2->1->0的y坐标单调递减则y1为奇点，若点1->2->3的y坐标单调递减则y2为奇点
        if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3)))
        {
            ymin++;
            x += dx;
        }
        //创建新边，插入边表ET
        Edge* p = new Edge();
        p->ymax = ymax;
        p->x = x;
        p->dx = dx;
        p->next = ET[ymin]->next;
        ET[ymin]->next = p;
    }

    //扫描线从下往上扫描，y坐标每次加1
    for (int i = 0; i < maxY; i++)
    {
        //取出ET中当前扫描行的所有边并按x的递增顺序（若x相等则按dx的递增顺序）插入AET
        while (ET[i]->next)
        {
            //取出ET中当前扫描行表头位置的边
            Edge* pInsert = ET[i]->next;
            Edge* p = AET;
            //在AET中搜索合适的插入位置
            while (p->next)
            {
                if (pInsert->x > p->next->x)
                {
                    p = p->next;
                    continue;
                }
                if (pInsert->x == p->next->x && pInsert->dx > p->next->dx)
                {
                    p = p->next;
                    continue;
                }
                //找到位置
                break;
            }
            //将pInsert从ET中删除，并插入AET的当前位置
            ET[i]->next = pInsert->next;
            pInsert->next = p->next;
            p->next = pInsert;
        }

        //AET中的边两两配对并填色
        Edge* p = AET;
        while (p->next && p->next->next)
        {
            for (int x = p->next->x; x < p->next->next->x; x++)
            {
                setPixel(x, i,penc);
                
            }
            p = p->next->next;
        }

        //删除AET中满足y=ymax的边
        p = AET;
        while (p->next)
        {
            if (p->next->ymax == i)
            {
                Edge* pDelete = p->next;
                p->next = pDelete->next;
                pDelete->next = nullptr;
                delete pDelete;
            }
            else
            {
                p = p->next;
            }
        }

        //更新AET中边的x值，进入下一循环
        p = AET;
        while (p->next)
        {
            p->next->x += p->next->dx;
            p = p->next;
        }
    }
   

    // 释放边表和活动边表占用的内存
    for (int i = 0; i < maxY; i++)
    {
        Edge* ptr = ET[i];
        if (ptr != nullptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }
    Edge* p = AET;
    while (p)
    {
        Edge* tmp = p->next;
        delete p;
        p = tmp;
    }
}

//z_buffer,对深度z填充
void Raster::polygonScan(Vertex3D* pts, int count, Color penc)
{
    //计算最高点的y坐标
    int maxY = 0;
    for (int i = 0; i < count; i++)
    {
        if (pts[i].pos.y() > maxY)
        {
            maxY = pts[i].pos.y();
        }
    }
    //初始化边表ET和活动边表AET

    Edge* ET2[3000];
    for (int i = 0; i < maxY; i++)
    {
        ET2[i] = new Edge();
        ET2[i]->next = nullptr;
    }
    Edge* AET2 = new Edge();
    AET2->next = nullptr;

    Edge* ET1[3000];
    for (int i = 0; i < maxY; i++)
    {
        ET1[i] = new Edge();
        ET1[i]->next = nullptr;
    }
    Edge* AET1 = new Edge();
    AET1->next = nullptr;

    //建立边表ET
    for (int i = 0; i < count; i++)
    {
        //取出当前点1前后相邻的共4个点，点1与点2的连线作为本次循环处理的边，另外两个点点0和点3用于计算奇点
        int x0 = pts[(i - 1 + count) % count].pos.x();
        int x1 = pts[i].pos.x();
        int x2 = pts[(i + 1) % count].pos.x();
        int x3 = pts[(i + 2) % count].pos.x();
        int y0 = pts[(i - 1 + count) % count].pos.y();
        int y1 = pts[i].pos.y();
        int y2 = pts[(i + 1) % count].pos.y();
        int y3 = pts[(i + 2) % count].pos.y();

        float z0 = pts[(i - 1 + count) % count].pos.z();
        float z1 = pts[i].pos.z();
        float z2 = pts[(i + 1) % count].pos.z();
        float z3 = pts[(i + 2) % count].pos.z();


        //水平线直接舍弃
        if (y1 == y2)
            continue;
        //分别计算下端点y坐标、上端点y坐标、下端点x坐标和斜率倒数
        int ymin = y1 > y2 ? y2 : y1;
        int ymax = y1 > y2 ? y1 : y2;
        float x = y1 > y2 ? x2 : x1;
        float dx = (x1 - x2) * 1.0f / (y1 - y2);

        float z = y1 > y2 ? z2 : z1;
        float dz = (z1 - z2) * 1.0f / (y1 - y2);
        //奇点特殊处理，若点2->1->0的y坐标单调递减则y1为奇点，若点1->2->3的y坐标单调递减则y2为奇点
        if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3)))
        {
            ymin++;
            x += dx;
            z += dz;
        }
        //创建新边，插入边表ET
        

        Edge* p1 = new Edge();
        p1->ymax = ymax;
        p1->z = z;
        p1->dz = dz;
        p1->next = ET1[ymin]->next;
        ET1[ymin]->next = p1;

        Edge* p2 = new Edge();
        p2->ymax = ymax;
        p2->x = x;
        p2->dx = dx;
        p2->next = ET2[ymin]->next;
        ET2[ymin]->next = p2;
    }

    //扫描线从下往上扫描，y坐标每次加1
    for (int i = 0; i < maxY; i++)
    {
        //取出ET中当前扫描行的所有边并按x的递增顺序（若x相等则按dx的递增顺序）插入AET
        while (ET2[i]->next)
        {
            //取出ET中当前扫描行表头位置的边
            Edge* pInsert2 = ET2[i]->next;
            Edge* p2 = AET2;
            //在AET中搜索合适的插入位置
            while (p2->next)
            {
                if (pInsert2->x > p2->next->x)
                {
                    p2 = p2->next;
                    continue;
                }
                if (pInsert2->x == p2->next->x && pInsert2->dx > p2->next->dx)
                {
                    p2 = p2->next;
                    continue;
                }
                //找到位置
                break;
            }
            //将pInsert从ET中删除，并插入AET的当前位置
            ET2[i]->next = pInsert2->next;
            pInsert2->next = p2->next;
            p2->next = pInsert2;
        }

        //取出ET中当前扫描行的所有边并按x的递增顺序（若x相等则按dx的递增顺序）插入AET
        while (ET1[i]->next)
        {
            //取出ET中当前扫描行表头位置的边
            Edge* pInsert1 = ET1[i]->next;
            Edge* p1 = AET1;
            //在AET中搜索合适的插入位置
            while (p1->next)
            {
                if (pInsert1->z > p1->next->z)
                {
                    p1 = p1->next;
                    continue;
                }
                if (pInsert1->z == p1->next->z && pInsert1->dz > p1->next->dz)
                {
                    p1 = p1->next;
                    continue;
                }
                //找到位置
                break;
            }
            //将pInsert从ET中删除，并插入AET的当前位置
            ET1[i]->next = pInsert1->next;
            pInsert1->next = p1->next;
            p1->next = pInsert1;
        }

        //AET中的边两两配对并填色
        Edge* p2 = AET2;
        Edge* p1 = AET1;
        while (p2->next && p2->next->next)
        {
            int cc = 1;
            for (int x = p2->next->x; x < p2->next->next->x; x++)
            {
                int xl = p2->next->x;
                int xr = p2->next->next->x;

                float zl = p1->next->z;
                float zr = p1->next->next->z;
                if ((xr - xl) != 0)
                {
                    float k = (zr - zl) / (xr - xl);
                    float z = cc*k + zl;
                    setPixel(x, i, z, penc);
                    cc += 1;
                }

            }
            p2 = p2->next->next;
            p1 = p1->next->next;
        }

        //删除AET中满足y=ymax的边
        p2 = AET2;
        while (p2->next)
        {
            if (p2->next->ymax == i)
            {
                Edge* pDelete2 = p2->next;
                p2->next = pDelete2->next;
                pDelete2->next = nullptr;
                delete pDelete2;
            }
            else
            {
                p2 = p2->next;
            }
        }
        p1 = AET1;
        while (p1->next)
        {
            if (p1->next->ymax == i)
            {
                Edge* pDelete1 = p1->next;
                p1->next = pDelete1->next;
                pDelete1->next = nullptr;
                delete pDelete1;
            }
            else
            {
                p1 = p1->next;
            }
        }

        //更新AET中边的x值，进入下一循环
        p2 = AET2;
        while (p2->next)
        {
            p2->next->x += p2->next->dx;
            p2 = p2->next;
        }
        //更新AET中边的x值，进入下一循环
        p1 = AET1;
        while (p1->next)
        {
            p1->next->z += p1->next->dz;
            p1 = p1->next;
        }
    }


    // 释放边表和活动边表占用的内存
    for (int i = 0; i < maxY; i++)
    {
        Edge* ptr2 = ET2[i];
        if (ptr2 != nullptr)
        {
            delete ptr2;
            ptr2 = nullptr;
        }
    }
    Edge* pp = AET2;
    while (pp)
    {
        Edge* tmp = pp->next;
        delete pp;
        pp = tmp;
    }

    // 释放边表和活动边表占用的内存
    for (int i = 0; i < maxY; i++)
    {
        Edge* ptr1 = ET1[i];
        if (ptr1 != nullptr)
        {
            delete ptr1;
            ptr1 = nullptr;
        }
    }
    Edge* ppt = AET1;
    while (ppt)
    {
        Edge* tmp1 = ppt->next;
        delete ppt;
        ppt = tmp1;
    }
}


void Raster::BoundaryFill(int x, int y, unsigned bdrColor, unsigned newColor)
{
    PixelPoint startPt = { x, y };
    PixelPoint AdjPt[4];
    vector<PixelPoint> AdjPtList;
    AdjPtList.push_back(startPt);
    while (!AdjPtList.empty())
    {
        PixelPoint topPt = AdjPtList.back();
        //g_State.drawPixelCB(topPt.x, topPt.y, newColor);
        setPixel(topPt.x, topPt.y, newColor);
        AdjPt[0] = { topPt.x - 1,topPt.y };
        AdjPt[1] = { topPt.x,topPt.y + 1 };
        AdjPt[2] = { topPt.x + 1,topPt.y };
        AdjPt[3] = { topPt.x, topPt.y - 1 };
        AdjPtList.pop_back();
        for (int i = 0; i < 4; i++)
        {
            unsigned oldColor = getPixel(AdjPt[i].x, AdjPt[i].y);
            if (oldColor != bdrColor && oldColor != newColor)
            {
                AdjPtList.push_back(AdjPt[i]);
            }
        }
    }
}

void Raster::PointFill(int x, int y, unsigned newColor)
{
    unsigned oldColor = getPixel(x, y); // 获取区域内点旧的颜色
    if (oldColor == newColor) return;
    PixelPoint startPt = { x, y };
    PixelPoint AdjPt[4];  // 存放四联通区域周边的四个点
    vector<PixelPoint> AdjPtList; // 设置一个栈存放未填充新颜色的点
    AdjPtList.push_back(startPt); // 将种子点压入栈

    // 从栈顶取出点填充新颜色，再检查周边四个点，若未填充压入栈中,循环直至栈为空
    while (!AdjPtList.empty())
    {
        PixelPoint topPt = AdjPtList.back();
        //g_State.drawPixelCB(topPt.x, topPt.y, newColor);
        setPixel(topPt.x, topPt.y, newColor);
        AdjPt[0] = { topPt.x - 1,topPt.y };
        AdjPt[1] = { topPt.x,topPt.y + 1 };
        AdjPt[2] = { topPt.x + 1,topPt.y };
        AdjPt[3] = { topPt.x, topPt.y - 1 };
        AdjPtList.pop_back();
        for (int i = 0; i < 4; i++)
        {
            if (getPixel(AdjPt[i].x, AdjPt[i].y) == oldColor)
            {
                AdjPtList.push_back(AdjPt[i]);
            }
        }
    }
}



/*多边形填充光栅化*/
void GetYMinMax(PixelPoint* data, int size, int& ymin, int& ymax)
{
    ymin = data[0].y;
    ymax = data[0].y;
    for (int i = 1; i < size; i++)
    {
        if (data[i].y > ymax)
            ymax = data[i].y;
        if (data[i].y < ymin)
            ymin = data[i].y;
    }
}
void InitET(vector<vector<State::tagEDGE>>& etEDGE, PixelPoint* data, int size, int ymin, int ymax)
{
    State::tagEDGE edgeNode;
    // 遍历所有点
    for (int i = 0; i < size; i++)
    {
        PixelPoint ps = data[i]; // 当前边起点
        PixelPoint pe = data[(i + 1) % size]; // 当前边终点
        PixelPoint pss = data[(i - 1 + size) % size]; // 起点相邻边起点
        PixelPoint pee = data[(i + 2) % size]; // 终点相邻边终点
        // 水平线不处理
        if (ps.y != pe.y)
        {
            edgeNode.dx = double(pe.x - ps.x) / double(pe.y - ps.y);
            if (pe.y > ps.y) // 取最小值y的点x
            {
                edgeNode.x = ps.x;
                edgeNode.ymax = pee.y >= pe.y ? pe.y - 1 : pe.y;
                etEDGE[ps.y - ymin].push_back(edgeNode);
            }
            else
            {
                edgeNode.x = pe.x;
                edgeNode.ymax = pss.y >= ps.y ? ps.y - 1 : ps.y;
                etEDGE[pe.y - ymin].push_back(edgeNode);
            }
        }
    }
}

// 升序排列
bool LessSort(State::tagEDGE a, State::tagEDGE b)
{
    if (a.x != b.x)
    {
        return a.x < b.x;
    }

    return a.dx < b.dx;
    //return (a.x < b.x ? true : (a.dx < b.dx ? true : false));
}
void InsertAET(vector<State::tagEDGE>& e, vector<State::tagEDGE>& ae)
{
    if (e.size())
    {
        ae.insert(ae.end(), e.begin(), e.end());
        sort(ae.begin(), ae.end(), LessSort);
    }
    return;
}
void ScanFillLine(vector<State::tagEDGE>& aetEDGE, int y, unsigned color)
{
    int size = aetEDGE.size();
    int i = 1;
    while (i < size)
    {
        int x0 = aetEDGE[i - 1].x;
        int x1 = aetEDGE[i].x;
        for (int x = x0; x <= x1; x++)
        {
            //g_State.drawPixelCB(x, y, color);
            setPixel(x, y, color);
        }

        i += 2;
    }
}
void RemoveAET(vector<State::tagEDGE>& aetEDGE, int y)
{
    auto iter = aetEDGE.begin();
    while (iter != aetEDGE.end())
    {
        if (y == (*iter).ymax)
        {
            iter = aetEDGE.erase(iter);
        }
        else
            iter++;
    }
}

void UpdateAET(vector<State::tagEDGE>& aetEDGE)
{
    for (vector<State::tagEDGE>::iterator iter = aetEDGE.begin(); iter != aetEDGE.end(); iter++)
    {
        (*iter).x += (*iter).dx;
    }
}

void ScanLineFill(vector<State::tagEDGE>& aetEDGE, vector<vector<State::tagEDGE>>& etEDGE, int ymin, int ymax, unsigned color)
{
    // 从最小y开始，逐行扫描至最大y
    for (int y = ymin; y <= ymax; y++)
    {
        InsertAET(etEDGE[y - ymin], aetEDGE); // 若当前边表中有活性边，则插入活性表中
        ScanFillLine(aetEDGE, y, color); // 对当前行进行填充
        RemoveAET(aetEDGE, y);  // 当前活性边表中是否有边已经扫描完，需要移除
        UpdateAET(aetEDGE); // 更新x坐标
        continue; // 继续下一行扫描
    }
}
void HorizonEdgeFill(PixelPoint* data, int size, unsigned color)
{
    for (int i = 0; i < size; i++)
    {
        if (data[i].y == data[(i + 1) % size].y)
        {
            for (int x = data[i].x; x < data[(i + 1) % size].x; x++)
            {
                //g_State.drawPixelCB(x, data[i].y, color);
                setPixel(x, data[i].y, color);
            }
        }
    }
}

void Raster::drawPolygon(PixelPoint* data, int size, unsigned color)
{
    //直接填充水平线
    HorizonEdgeFill(data, size, color);
    int ymin = 0;
    int ymax = 0;
    GetYMinMax(data, size, ymin, ymax); // 获取坐标点最大最小y值
    vector<State::tagEDGE> aetEDGE; // 活性边表AET
    vector<vector<State::tagEDGE>> etEDGE(ymax - ymin + 1); // 边表
    InitET(etEDGE, data, size, ymin, ymax); // 初始化边表

    ScanLineFill(aetEDGE, etEDGE, ymin, ymax, color); // 进行扫描
}

/* 画圆对应八个点位置*/
void CirclePlot(int xCenter, int yCenter, int x, int y, unsigned color)
{
    setPixel(xCenter + x, yCenter + y, color);
    setPixel(xCenter - x, yCenter + y, color);
    setPixel(xCenter + x, yCenter - y, color);
    setPixel(xCenter - x, yCenter - y, color);
    setPixel(xCenter + y, yCenter + x, color);
    setPixel(xCenter - y, yCenter + x, color);
    setPixel(xCenter + y, yCenter - x, color);
    setPixel(xCenter - y, yCenter - x, color);
}
void Raster::drawCircle(double x, double y, double r, unsigned color)
{
    // MidBresenham algorithm
    double x0, y0, d;
    x0 = 0;
    y0 = r;
    d = 1 - r;
    CirclePlot(round(x), round(y), round(x0), round(y0), color);
    while (x0 <= y0)
    {
        if (d < 0)
        {
            d += 2 * x0 + 3;
        }
        else
        {
            d += 2 * (x0 - y0) + 5;
            y0--;
        }
        x0++;
        CirclePlot(round(x), round(y), round(x0), round(y0), color);
    }
}

double ToGetDeepthZ(Vec4 pt0, Vec4 pt1, Vec4 pt2, int x, int y)
{
    // 多边形平面方程
    // Ax+By+Cz+D=0
    // z(x,y)=(-Ax-By-D)/C

    // 平面的法向量(A,B,C)
    //| i      j    k   |
    //|x2-x1 y2-y1 z2-z1| //mx my mz
    //|x3-x1 y3-y1 z3-z1| //nx ny nz
    double mx = pt1.x() - pt0.x();
    double my = pt1.y() - pt0.y();
    double mz = pt1.z() - pt0.z();

    double nx = pt2.x() - pt0.x();
    double ny = pt2.y() - pt0.y();
    double nz = pt2.z() - pt0.z();

    double A = my * nz - ny * mz;
    double B = nx * mz - mx * nz;
    double C = mx * ny - nx * my;
    double D = -(A * pt0.x() + B * pt0.y() + C * pt0.z());

    if (C == 0)//此时多边形在xoy平面上的投影为线，不考虑
        return 9999;
    double  z = (-A * x - B * y - D) / C;
    return z;
}
struct Codepoint {
    int x, y;
    char code[4];
    //构造函数
    Codepoint() {}
    //构造函数重载
    Codepoint(PixelPoint pt) {
        this->x = pt.x;
        this->y = pt.y;
    }
};
//划分矩形区域并对各区域进行编码
// 0101 | 0100 | 0110
// ——————————
// 0001 | 0000 | 0010
// ——————————
// 1001 | 1000 | 1010
//底上右左
Codepoint setCode(Codepoint pt, int x_max, int x_min, int y_max, int y_min) {
    Codepoint temp_pt;
    if (pt.y < y_min)
        temp_pt.code[0] = '1';
    else
        temp_pt.code[0] = '0';
    if (pt.y > y_max)
        temp_pt.code[1] = '1';
    else
        temp_pt.code[1] = '0';
    if (pt.x > x_max)
        temp_pt.code[2] = '1';
    else
        temp_pt.code[2] = '0';
    if (pt.x < x_min)
        temp_pt.code[3] = '1';
    else
        temp_pt.code[3] = '0';
    temp_pt.x = pt.x;
    temp_pt.y = pt.y;
    return temp_pt;
}
//判断直线是否可见（需要裁剪）
//0保留，1舍弃，2裁剪
//flag表示编码code[]里是否有1
int DisplayLine(Codepoint pt1, Codepoint pt2) {
    int flag = 0, i;
    for (i = 0; i < 3; i++) {
        //判断code里是否有1
        if (pt1.code[i] == '1' || pt2.code[i] == '1')
            flag = 1;
    }

    //通过flag对3种情况进行判断
    if (flag == 0)
        //flag==0，code里不含1，则直线在0000内部，全部保留；
        return 0;
    else {
        for (i = 0; i < 3; i++) {
            //有相同编码位为1，则不经过0000，舍弃；
            if ((pt1.code[i] == pt2.code[i]) && pt1.code[i] == '1')
                return 1;
        }
        //code里有1，但经过0000，需要裁剪。
        return 2;
    }
}
//交点代替端点
//总是将pt1设置为直线与0000的交点
PixelPoint SetEndpt(Codepoint pt1, Codepoint pt2, int x_max, int x_min, int y_max, int y_min) {
    PixelPoint pt;
    Codepoint temp;//直线与0000的交点
    int x, y;//x为直线与最近的在0000边界线在x方向上的坐标，y为y方向上的
    float k, m;//k为直线斜率，m为直线与0000交点的坐标
    //裁剪左右两边的线段
    if (pt1.code[3] == '1')
        x = x_min;//pt1在左边
    if (pt1.code[2] == '1')
        x = x_max;//pt1在右边
    if (pt1.code[3] == '1' || pt1.code[2] == '1') {
        //pt1不在0000内
        k = (float)(pt1.y - pt2.y) / (pt1.x - pt2.x);
        m = (float)pt1.y + k * (x - pt1.x);
        temp.x = x;
        temp.y = m;
        if (temp.y<y_max && temp.y>y_min) {
            //新点的y坐标正好在0000内，裁剪结束
            pt.x = temp.x;
            pt.y = temp.y;
            return pt;
        }
    }
    //裁剪上下两边的线段
    if (pt1.code[0] == '1')
        y = y_min;//pt1在底部
    if (pt1.code[1] == '1')
        y = y_max;//pt1在上部
    if (pt1.code[0] == '1' || pt1.code[1] == '1') {
        k = (float)(pt1.y - pt2.y) / (pt1.x - pt2.x);
        m = (float)pt1.x + (y - pt1.y) / k;
        temp.x = m;
        temp.y = y;
        for (int i = 0; i < 3; i++) {
            temp.code[i] = pt1.code[i];
        }
        //再裁剪一次
        pt.x = temp.x;
        pt.y = temp.y;
        return pt;
    }
    else
    {
        //pt1在0000内部，直接返回
        pt.x = pt1.x;
        pt.y = pt1.y;
        return pt;
    }
}
void Raster::CohenSutherland(PixelPoint pt1, PixelPoint pt2, Layer* container) {
    //确定区域0000范围
    int x_max = pt1.x > pt2.x ? pt1.x : pt2.x;
    int x_min = pt1.x < pt2.x ? pt1.x : pt2.x;
    int y_max = pt1.y > pt2.y ? pt1.y : pt2.y;
    int y_min = pt1.y < pt2.y ? pt1.y : pt2.y;
    vector<Geometry*> TailorGeo;
    int i = 0;
    while (i < container->getGeometryCount()) {
        //进行循环，确定每一条线，并进行编码
        Geometry* pSymbolGeometry = (*container)[i];
        if (pSymbolGeometry->getGeomType() != gtPolyline)
        {
            i++;
            continue;
        }
        PixelPoint Lpt1, Lpt2;
        PolylineGeometry* pPolylineGeometry = (PolylineGeometry*)pSymbolGeometry;
        const vector<Point2D>& pts = pPolylineGeometry->getPts();
        PolylineGeometry* nPolylineGeometry = new PolylineGeometry();
        for (int i = 0; i < (pts.size() - 1); i++)
        {
            //得到线的两个端点坐标
            Lpt1.x = pts[i].x;
            Lpt1.y = pts[i].y;
            Lpt2.x = pts[i + 1].x;
            Lpt2.y = pts[i + 1].y;

            PixelPoint pt3, pt4;
            Codepoint pt1, pt2;
            pt1 = setCode(Codepoint(Lpt1), x_max, x_min, y_max, y_min);
            pt2 = setCode(Codepoint(Lpt2), x_max, x_min, y_max, y_min);
            int display_result = DisplayLine(pt1, pt2);
            switch (display_result)
            {
            case 0://保留
                nPolylineGeometry->addPoint(Lpt1.x, Lpt1.y);
                nPolylineGeometry->addPoint(Lpt2.x, Lpt2.y);
                break;
            case 1://舍弃
                break;
            case 2://裁剪
                pt3 = SetEndpt(pt1, pt2, x_max, x_min, y_max, y_min);
                pt4 = SetEndpt(pt2, pt1, x_max, x_min, y_max, y_min);
                nPolylineGeometry->addPoint(pt3.x, pt3.y);
                nPolylineGeometry->addPoint(pt4.x, pt4.y);
                break;
            }
        }
        if (nPolylineGeometry->getPts().size()) TailorGeo.push_back(nPolylineGeometry);
        container->DeleteGeo(i);

    }
    for (int i = 0; i < TailorGeo.size(); i++)
    {
        container->addGeometry(TailorGeo[i]);
    }
    TailorGeo.clear();
}


//判断线段端点在裁剪端口矩形的里面还是外面
int LptOUTERrect(PixelPoint Lpt, PixelPoint rect[2]) {
    //确定范围
    int x_min, x_max, y_min, y_max;
    x_min = rect[0].x < rect[1].x ? rect[0].x : rect[1].x;
    x_max = rect[0].x > rect[1].x ? rect[0].x : rect[1].x;
    y_min = rect[0].y < rect[1].y ? rect[0].y : rect[1].y;
    y_max = rect[0].y > rect[1].y ? rect[0].y : rect[1].y;
    if (Lpt.x > x_min && Lpt.x<x_max && Lpt.y>y_min && Lpt.y < y_max)
        return 0;
    return 1;
}
//二分逼近法查找确定直线段和窗口的交点
PixelPoint BSearch_findIntersection(PixelPoint outsidePt, PixelPoint insidePt, PixelPoint* rect) {

    PixelPoint mid; //定义中点坐标
    PixelPoint pt1, pt2;
    pt1 = insidePt;
    pt2 = outsidePt;
    mid.x = (insidePt.x + outsidePt.x) / 2;
    mid.y = (insidePt.y + outsidePt.y) / 2;
    int mid_outside = LptOUTERrect(mid, rect);
    while (abs(pt2.x - pt1.x) > 2 || abs(pt2.y - pt1.y) > 2) {
        if (mid_outside) {
            pt2 = mid;
        }
        else {
            pt1 = mid;
        }
        mid.x = (pt1.x + pt2.x) / 2;
        mid.y = (pt1.y + pt2.y) / 2;
        mid_outside = LptOUTERrect(mid, rect);
    }
    return pt1;
}
//判断直线与对角线是否平行，若平行则返回1，无交点
//向量法，叉乘，(x1,y1)×(x2,y2)=x1*y2-x2*y1>0，则在顺时针方向；<0，则在逆时针方向
int judgeParallel(PixelPoint Lpt1, PixelPoint Lpt2, PixelPoint DiagonalPt1, PixelPoint DiagonalPt2) {
    int flag1, flag2;
    //先判断线段两个端点在对角线的异侧
    int d1 = DiagonalPt1.x * Lpt1.y - DiagonalPt1.y * Lpt1.x;
    int d2 = DiagonalPt1.x * Lpt2.y - DiagonalPt1.y * Lpt2.x;
    if (d1 * d2 <= 0) {
        //不平行，相交
        flag1 = 0;
    }
    flag1 = 1;
    //再判断对角线两个端点在线段的异侧
    int d3 = Lpt1.x * DiagonalPt1.y - Lpt1.y * DiagonalPt1.x;
    int d4 = Lpt1.x * DiagonalPt2.y - Lpt1.y * DiagonalPt2.x;
    if (d3 * d4 <= 0) {
        flag2 = 0;
    }
    flag2 = 1;
    if (flag1 == 0 && flag2 == 0)
        return 0;//不平行，相交，返回0
    else
        return 1;
}
//求线段与对角线的交点
PixelPoint getPoint(PixelPoint out1, PixelPoint out2, PixelPoint point1, PixelPoint point11)
{
    int a = out2.x - out1.x;
    int b = point1.x - point11.x;
    int c = out2.y - out1.y;
    int d = point1.y - point11.y;
    int g = point1.x - out1.x;
    int h = point1.y - out1.y;
    int f = a * d - b * c;
    double t = double((d * g - b * h)) / double(f);
    PixelPoint m;
    m.x = out1.x + t * (out2.x - out1.x);
    m.y = out1.y + t * (out2.y - out1.y);
    return m;
}
//中点裁剪算法
void Raster::MidPointClipAlgorithm(PixelPoint pt1, PixelPoint pt2, Layer* container) {
    //确定裁剪端口矩形
    PixelPoint rect[2];
    rect[0].x = pt1.x < pt2.x ? pt1.x : pt2.x;
    rect[0].y = pt1.y < pt2.y ? pt1.y : pt2.y;
    rect[1].x = pt1.x > pt2.x ? pt1.x : pt2.x;
    rect[1].y = pt1.y > pt2.y ? pt1.y : pt2.y;
    vector<Geometry*> TailorGeo;
    int i = 0;
    //遍历图层
    while (i < container->getGeometryCount()) {
        //线段的两个端点Lpt1,Lpt2
        PixelPoint Lpt1, Lpt2;
        Geometry* pGeometry = (*container)[i];
        if (pGeometry->getGeomType() != gtPolyline)
        {
            i++;
            continue;
        }
        PolylineGeometry* pPolylineGeometry = (PolylineGeometry*)pGeometry;
        const vector<Point2D>& pts = pPolylineGeometry->getPts();
        Lpt1.x = pts[0].x, Lpt1.y = pts[0].y;
        Lpt2.x = pts[1].x, Lpt2.y = pts[1].y;
        int codeLpt1 = LptOUTERrect(Lpt1, rect);
        int codeLpt2 = LptOUTERrect(Lpt2, rect);
        PixelPoint out1, out2, inn1, inn2, mid;
        if (codeLpt1 == 0 && codeLpt2 == 0) {
            //线段两个端点都在矩形内
            inn1 = Lpt1;
            inn2 = Lpt2;
        }
        if ((codeLpt1 == 1 && codeLpt2 == 0) || (codeLpt1 == 0 && codeLpt2 == 1)) {
            //线段只有一个端点在矩形内
            if (codeLpt1) {
                //Lpt1在外部
                inn1 = Lpt2;
                out1 = Lpt1;

            }
            else {
                //Lpt1在内部
                inn1 = Lpt1;
                out1 = Lpt2;
            }
            inn2 = BSearch_findIntersection(out1, inn1, rect);
        }
        if (codeLpt1 == 1 && codeLpt2 == 1) {
            //线段端点都在矩形外
            out1 = Lpt1;
            out2 = Lpt2;
            inn1 = BSearch_findIntersection(out1, out2, rect);
            inn2 = BSearch_findIntersection(out2, out1, rect);
            //利用矩形对角线与线段的交点diagonal_pt代替线段的中点mid
            //定义矩形的四个顶点abcd，则对角线为ad和bc
            PixelPoint a, b, c, d;
            a.x = rect[0].x, a.y = rect[1].y;
            b = rect[0];
            c = rect[1];
            d.x = rect[1].x, d.y = rect[0].y;
            int codeDiagonal1, codeDiagonal2;
            codeDiagonal1 = judgeParallel(out1, out2, a, d);
            codeDiagonal2 = judgeParallel(out1, out2, b, c);
            if (codeDiagonal1 == 0 || codeDiagonal2 == 0) {
                //相交，求交点
                PixelPoint diagonal_pt;
                if (!codeDiagonal1)
                    diagonal_pt = getPoint(out1, out2, a, d);
                else
                    diagonal_pt = getPoint(out1, out2, b, c);
                inn1 = inn2 = diagonal_pt;
                inn1 = BSearch_findIntersection(out1, inn1, rect);
                inn2 = BSearch_findIntersection(out2, inn2, rect);
            }
            else
            {
                //出现这种情况的原因是线段与线框没有交点，所以将inn都设置为空
                inn1.x = inn1.y = inn2.x = inn2.y = 999;
            }
        }
        if (inn1.x != 999 && inn2.x != 999) {
            PolylineGeometry* nPolylineGeometry = new PolylineGeometry();
            nPolylineGeometry->addPoint(inn1.x, inn1.y);
            nPolylineGeometry->addPoint(inn2.x, inn2.y);
            TailorGeo.push_back(nPolylineGeometry);
            //container->DeleteGeo(i);
        }
        container->DeleteGeo(i);
    }
    if (TailorGeo.size() == 0) return;
    //container->DeleteGeometry();
    for (int i = 0; i < TailorGeo.size(); i++)
    {
        container->addGeometry(TailorGeo[i]);
    }
}
//多边形的裁剪
//构建一个新的点的类
struct point
{
public:
    double x, y;
    point()
    {
        this->x = this->y = 0;
    }
    point(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
    void setX(double x)
    {
        this->x = x;
    }
    void setY(double y)
    {
        this->y = y;
    }
    double getX()
    {
        return this->x;
    }
    double getY()
    {
        return this->y;
    }
};
//再定义一个向量的类
struct VectorLine :point
{
    VectorLine(point start, point end)
    {
        this->start = start;
        this->end = end;
    }
    point start;   //开始顶点
    point end; //终结点
};
double Multi(point pt, point v_start, point v_end) {
    int x1 = pt.x - v_start.x;
    int y1 = pt.y - v_start.y;
    int x2 = v_end.x - v_start.x;
    int y2 = v_end.y - v_start.y;
    int km = x1 * y2 - x2 * y1;
    //P(x1,y1),Q(x2,y2)
    //P×Q>0,P在Q的顺时针方向
    //P×Q<0,P在Q的逆时针方向
    return km;
}
//点是否在边界线内侧
int isInside(point& pt, VectorLine& v) {
    int k = Multi(pt, v.start, v.end) >= 0 ? 0 : 1;
    //由于裁剪矩形向量是逆时针的，左侧即在矩形内部，即Multi>0在内侧，返回1
    return k;
}
//求边界与多边形边的交点
point IntersectionPt(point startPt1, point endPt1, point startPt2, point endPt2) {
    double pX = (Multi(startPt1, endPt2, startPt2) * endPt1.x - Multi(endPt1, endPt2, startPt2) * startPt1.x) /
        (Multi(startPt1, endPt2, startPt2) - Multi(endPt1, endPt2, startPt2));
    double pY = (Multi(startPt1, endPt2, startPt2) * endPt1.y - Multi(endPt1, endPt2, startPt2) * startPt1.y) /
        (Multi(startPt1, endPt2, startPt2) - Multi(endPt1, endPt2, startPt2));
    return point(pX, pY);
}
//凸多边形裁剪算法
//rectPt1为矩形裁剪框的左上角顶点
//rectPt2为矩形裁剪框的右下角顶点
void Raster::Sutherland_Hodgeman(PixelPoint rectPt1, PixelPoint rectPt2, Layer* container) {
    //确定裁剪范围
    int x_min = rectPt1.x;
    int x_max = rectPt2.x;
    int y_min = rectPt2.y;
    int y_max = rectPt1.y;
    vector<point>points;
    vector<Geometry*> TailorGeo;
    int i = 0;
    //遍历图层
    while (i < container->getGeometryCount()) {
        Geometry* pGeometry = (*container)[i];
        if (pGeometry->getGeomType() != gtPolygon)
        {
            i++;
            continue;
        }
        PolygonGeometry* pPolygonGeometry = (PolygonGeometry*)pGeometry;
        const vector<Point2D>& pts = pPolygonGeometry->getPts();
        if (pts.size() > 0)
        {
            for (int j = 0; j < pts.size(); j++)
                points.push_back(point(pts[j].x, pts[j].y)); //将点存在一个LIST中
        }
        points.push_back(point(pts[0].x, pts[0].y));
        //裁剪矩形边界改为向量，逆时针
        vector<VectorLine> vectors;
        vectors.push_back(VectorLine(point(x_max, y_min), point(x_max, y_max)));
        vectors.push_back(VectorLine(point(x_max, y_max), point(x_min, y_max)));
        vectors.push_back(VectorLine(point(x_min, y_max), point(x_min, y_min)));
        vectors.push_back(VectorLine(point(x_min, y_min), point(x_max, y_min)));
        vector<point> result, cur;
        int vectorsSize = vectors.size();
        int pointSize = points.size();
        if (points.size() > 0) {
            //点S为多边形存储的最后一个顶点，即为判断内外的起始点
            //point S = points[points.size()-1];
            point S = points[0];

            for (int j = 0; j < points.size(); ++j)
                result.push_back(points[j]);
            //flag：S在边界内侧，在就返回1
            int flag;
            //从最右边边开始，右上左下
            for (int j = 0; j < vectors.size(); ++j)
            {
                //先判断起始点S是否在边界线内
                if (isInside(S, vectors[j]))
                    flag = 1;
                else
                    flag = 0;
                //当前矩形边时的所有多边形的点是否在边的内侧
                int resultSize = result.size();
                for (int k = 0; k < resultSize; ++k)
                {
                    //当前点在内侧
                    if (isInside(result[k], vectors[j]))
                    {
                        //flag表示前一个点，flag==1表示在内侧
                        if (flag == 0)
                        {
                            //前一个点在外侧，当前点在内侧，求交点并存入cur中
                            flag = 1;
                            if (k == 0)
                                cur.push_back(IntersectionPt(S, result[k], vectors[j].start, vectors[j].end));
                            else
                                cur.push_back(IntersectionPt(result[k - 1], result[k], vectors[j].start, vectors[j].end));
                            cur.push_back(result[k]);
                        }
                        else {
                            //两个点都在内侧，直接保存当前点到cur中
                            cur.push_back(result[k]);
                        }
                    }
                    else {//当前点在外侧
                        if (flag) {
                            //前一个点在内侧，求交点并存入cur中
                            flag = 0;
                            if (k == 0)
                                cur.push_back(IntersectionPt(S, result[k], vectors[j].start, vectors[j].end));
                            else
                                cur.push_back(IntersectionPt(result[k - 1], result[k], vectors[j].start, vectors[j].end));
                        }
                        //两个点都在外侧
                        //将当前顶点赋给S，新的输入点
                        //S = result[k];
                    }
                }
                //if (j != 0) {
                //	//向cur中添加上一步未添加的S点
                //	cur.push_back(S);
                //}
                //cur.push_back(S);
                if (!cur.size())
                {
                    result.clear();
                    break;
                }
                if (cur[0].x != cur[cur.size() - 1].x || cur[0].y != cur[cur.size() - 1].y)
                {
                    cur.push_back(cur[0]);
                }

                if (j == vectors.size() - 1) {
                    //结束裁剪，将首尾点相连
                    result.clear();
                    for (int k = 0; k < cur.size() - 1; ++k) {
                        result.push_back(cur[k]);
                    }
                    //result.push_back(cur[0]);
                    cur.clear();
                }
                else {
                    S = cur[0];
                    int reaultLen = cur.size();
                    result.clear();
                    for (int k = 0; k < reaultLen; ++k)
                    {
                        result.push_back(cur[k]);
                    }
                    cur.clear();
                }
            }

            int K = result.size();
            if (K < 3) continue;
            PolygonGeometry* npPolygonGeometry = new PolygonGeometry();
            for (int j = 0; j < K; ++j)
            {
                npPolygonGeometry->addPoint(result[j].x, result[j].y);
            }
            TailorGeo.push_back(npPolygonGeometry);
            result.clear();
            vectors.clear();
            points.clear();
        }
        container->DeleteGeo(i);
    }
    if (TailorGeo.size() == 0) return;
    //container->DeleteGeometry();
    for (int i = 0; i < TailorGeo.size(); i++)
    {
        container->addGeometry(TailorGeo[i]);
    }
}