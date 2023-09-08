#include"1_Rasterprint.h"
#include <stdlib.h>
#include"Graphic.h"
#include"1_Grid.h"
#include "GeoDefine.h"
#include"1_state.h"

#include <stack>
Raster g_Raster;

//DDA�㷨
void Raster::DDA_Line(int x1, int y1, int x2, int y2,Color penc)
{
	float increx, increy, x, y, length;//��������
	int i;
	if (abs(x2 - x1) > abs(y2 - y1))//�ж����ĸ�������
		length = abs(x2 - x1);
	else
		length = abs(y2 - y1);
	increx = (x2 - x1) / length;//����������һ��Ϊ1��һ��Ϊk
	increy = (y2 - y1) / length;
	x = x1, y = y1;//���
	for (i = 1; i <= length; i++)
    {
        //���������
		if (g_State.DrawMode != dmPixel)
		{
            int xx = int(x + 0.5);
            int yy = int(y + 0.5);
			g_Grid.pixelToGrid(xx, yy, xx, yy);//ÿ������תΪ��Ӧ���к�
            g_Grid.drawCell(xx, yy, BLACK);
			x += increx;//x+����
			y += increy;//y+����
		}
        else 
        {
            setPixel(int(x + 0.5), int(y + 0.5), penc);//��Ϊû�а�����ص㣬������Ҫǿ��ת��Ϊ����
            x += increx;//x+����
            y += increy;//y+����

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


//�����ض����,ÿ����ʹ�ù�դ����ʽ�����ػ���
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

//Xɨ�����������
//�������ڱ߱�ET�ͻ�߱�AET��ͨ����Edge,z��xͨ��
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

//�����Xɨ���߻��Ա߱����
void Raster::polygonScan(PixelPoint* pts, int count, Color penc)
{
    //������ߵ��y����
    int maxY = 0;
    for (int i = 0; i < count; i++)
    {
        if (pts[i].y > maxY)
        {
            maxY = pts[i].y;
        }
    }
    //��ʼ���߱�ET�ͻ�߱�AET
    
    Edge* ET[1800];
    for (int i = 0; i < maxY; i++)
    {
        ET[i] = new Edge();
        ET[i]->next = nullptr;
    }
    Edge* AET = new Edge();
    AET->next = nullptr;

    //�����߱�ET
    for (int i = 0; i < count; i++)
    {
        //ȡ����ǰ��1ǰ�����ڵĹ�4���㣬��1���2��������Ϊ����ѭ������ıߣ������������0�͵�3���ڼ������
        int x0 = pts[(i - 1 + count) % count].x;
        int x1 = pts[i].x;
        int x2 = pts[(i + 1) % count].x;
        int x3 = pts[(i + 2) % count].x;
        int y0 = pts[(i - 1 + count) % count].y;
        int y1 = pts[i].y;
        int y2 = pts[(i + 1) % count].y;
        int y3 = pts[(i + 2) % count].y;

       
        //ˮƽ��ֱ������
        if (y1 == y2)
            continue;
        //�ֱ�����¶˵�y���ꡢ�϶˵�y���ꡢ�¶˵�x�����б�ʵ���
        int ymin = y1 > y2 ? y2 : y1;
        int ymax = y1 > y2 ? y1 : y2;
        float x = y1 > y2 ? x2 : x1;
        float dx = (x1 - x2) * 1.0f / (y1 - y2);
        //������⴦������2->1->0��y���굥���ݼ���y1Ϊ��㣬����1->2->3��y���굥���ݼ���y2Ϊ���
        if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3)))
        {
            ymin++;
            x += dx;
        }
        //�����±ߣ�����߱�ET
        Edge* p = new Edge();
        p->ymax = ymax;
        p->x = x;
        p->dx = dx;
        p->next = ET[ymin]->next;
        ET[ymin]->next = p;
    }

    //ɨ���ߴ�������ɨ�裬y����ÿ�μ�1
    for (int i = 0; i < maxY; i++)
    {
        //ȡ��ET�е�ǰɨ���е����б߲���x�ĵ���˳����x�����dx�ĵ���˳�򣩲���AET
        while (ET[i]->next)
        {
            //ȡ��ET�е�ǰɨ���б�ͷλ�õı�
            Edge* pInsert = ET[i]->next;
            Edge* p = AET;
            //��AET���������ʵĲ���λ��
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
                //�ҵ�λ��
                break;
            }
            //��pInsert��ET��ɾ����������AET�ĵ�ǰλ��
            ET[i]->next = pInsert->next;
            pInsert->next = p->next;
            p->next = pInsert;
        }

        //AET�еı�������Բ���ɫ
        Edge* p = AET;
        while (p->next && p->next->next)
        {
            for (int x = p->next->x; x < p->next->next->x; x++)
            {
                setPixel(x, i,penc);
                
            }
            p = p->next->next;
        }

        //ɾ��AET������y=ymax�ı�
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

        //����AET�бߵ�xֵ��������һѭ��
        p = AET;
        while (p->next)
        {
            p->next->x += p->next->dx;
            p = p->next;
        }
    }
   

    // �ͷű߱�ͻ�߱�ռ�õ��ڴ�
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

//z_buffer,�����z���
void Raster::polygonScan(Vertex3D* pts, int count, Color penc)
{
    //������ߵ��y����
    int maxY = 0;
    for (int i = 0; i < count; i++)
    {
        if (pts[i].pos.y() > maxY)
        {
            maxY = pts[i].pos.y();
        }
    }
    //��ʼ���߱�ET�ͻ�߱�AET

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

    //�����߱�ET
    for (int i = 0; i < count; i++)
    {
        //ȡ����ǰ��1ǰ�����ڵĹ�4���㣬��1���2��������Ϊ����ѭ������ıߣ������������0�͵�3���ڼ������
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


        //ˮƽ��ֱ������
        if (y1 == y2)
            continue;
        //�ֱ�����¶˵�y���ꡢ�϶˵�y���ꡢ�¶˵�x�����б�ʵ���
        int ymin = y1 > y2 ? y2 : y1;
        int ymax = y1 > y2 ? y1 : y2;
        float x = y1 > y2 ? x2 : x1;
        float dx = (x1 - x2) * 1.0f / (y1 - y2);

        float z = y1 > y2 ? z2 : z1;
        float dz = (z1 - z2) * 1.0f / (y1 - y2);
        //������⴦������2->1->0��y���굥���ݼ���y1Ϊ��㣬����1->2->3��y���굥���ݼ���y2Ϊ���
        if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3)))
        {
            ymin++;
            x += dx;
            z += dz;
        }
        //�����±ߣ�����߱�ET
        

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

    //ɨ���ߴ�������ɨ�裬y����ÿ�μ�1
    for (int i = 0; i < maxY; i++)
    {
        //ȡ��ET�е�ǰɨ���е����б߲���x�ĵ���˳����x�����dx�ĵ���˳�򣩲���AET
        while (ET2[i]->next)
        {
            //ȡ��ET�е�ǰɨ���б�ͷλ�õı�
            Edge* pInsert2 = ET2[i]->next;
            Edge* p2 = AET2;
            //��AET���������ʵĲ���λ��
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
                //�ҵ�λ��
                break;
            }
            //��pInsert��ET��ɾ����������AET�ĵ�ǰλ��
            ET2[i]->next = pInsert2->next;
            pInsert2->next = p2->next;
            p2->next = pInsert2;
        }

        //ȡ��ET�е�ǰɨ���е����б߲���x�ĵ���˳����x�����dx�ĵ���˳�򣩲���AET
        while (ET1[i]->next)
        {
            //ȡ��ET�е�ǰɨ���б�ͷλ�õı�
            Edge* pInsert1 = ET1[i]->next;
            Edge* p1 = AET1;
            //��AET���������ʵĲ���λ��
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
                //�ҵ�λ��
                break;
            }
            //��pInsert��ET��ɾ����������AET�ĵ�ǰλ��
            ET1[i]->next = pInsert1->next;
            pInsert1->next = p1->next;
            p1->next = pInsert1;
        }

        //AET�еı�������Բ���ɫ
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

        //ɾ��AET������y=ymax�ı�
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

        //����AET�бߵ�xֵ��������һѭ��
        p2 = AET2;
        while (p2->next)
        {
            p2->next->x += p2->next->dx;
            p2 = p2->next;
        }
        //����AET�бߵ�xֵ��������һѭ��
        p1 = AET1;
        while (p1->next)
        {
            p1->next->z += p1->next->dz;
            p1 = p1->next;
        }
    }


    // �ͷű߱�ͻ�߱�ռ�õ��ڴ�
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

    // �ͷű߱�ͻ�߱�ռ�õ��ڴ�
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
    unsigned oldColor = getPixel(x, y); // ��ȡ�����ڵ�ɵ���ɫ
    if (oldColor == newColor) return;
    PixelPoint startPt = { x, y };
    PixelPoint AdjPt[4];  // �������ͨ�����ܱߵ��ĸ���
    vector<PixelPoint> AdjPtList; // ����һ��ջ���δ�������ɫ�ĵ�
    AdjPtList.push_back(startPt); // �����ӵ�ѹ��ջ

    // ��ջ��ȡ�����������ɫ���ټ���ܱ��ĸ��㣬��δ���ѹ��ջ��,ѭ��ֱ��ջΪ��
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



/*���������դ��*/
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
    // �������е�
    for (int i = 0; i < size; i++)
    {
        PixelPoint ps = data[i]; // ��ǰ�����
        PixelPoint pe = data[(i + 1) % size]; // ��ǰ���յ�
        PixelPoint pss = data[(i - 1 + size) % size]; // ������ڱ����
        PixelPoint pee = data[(i + 2) % size]; // �յ����ڱ��յ�
        // ˮƽ�߲�����
        if (ps.y != pe.y)
        {
            edgeNode.dx = double(pe.x - ps.x) / double(pe.y - ps.y);
            if (pe.y > ps.y) // ȡ��Сֵy�ĵ�x
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

// ��������
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
    // ����Сy��ʼ������ɨ�������y
    for (int y = ymin; y <= ymax; y++)
    {
        InsertAET(etEDGE[y - ymin], aetEDGE); // ����ǰ�߱����л��Աߣ��������Ա���
        ScanFillLine(aetEDGE, y, color); // �Ե�ǰ�н������
        RemoveAET(aetEDGE, y);  // ��ǰ���Ա߱����Ƿ��б��Ѿ�ɨ���꣬��Ҫ�Ƴ�
        UpdateAET(aetEDGE); // ����x����
        continue; // ������һ��ɨ��
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
    //ֱ�����ˮƽ��
    HorizonEdgeFill(data, size, color);
    int ymin = 0;
    int ymax = 0;
    GetYMinMax(data, size, ymin, ymax); // ��ȡ����������Сyֵ
    vector<State::tagEDGE> aetEDGE; // ���Ա߱�AET
    vector<vector<State::tagEDGE>> etEDGE(ymax - ymin + 1); // �߱�
    InitET(etEDGE, data, size, ymin, ymax); // ��ʼ���߱�

    ScanLineFill(aetEDGE, etEDGE, ymin, ymax, color); // ����ɨ��
}

/* ��Բ��Ӧ�˸���λ��*/
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
    // �����ƽ�淽��
    // Ax+By+Cz+D=0
    // z(x,y)=(-Ax-By-D)/C

    // ƽ��ķ�����(A,B,C)
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

    if (C == 0)//��ʱ�������xoyƽ���ϵ�ͶӰΪ�ߣ�������
        return 9999;
    double  z = (-A * x - B * y - D) / C;
    return z;
}
struct Codepoint {
    int x, y;
    char code[4];
    //���캯��
    Codepoint() {}
    //���캯������
    Codepoint(PixelPoint pt) {
        this->x = pt.x;
        this->y = pt.y;
    }
};
//���־������򲢶Ը�������б���
// 0101 | 0100 | 0110
// ��������������������
// 0001 | 0000 | 0010
// ��������������������
// 1001 | 1000 | 1010
//��������
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
//�ж�ֱ���Ƿ�ɼ�����Ҫ�ü���
//0������1������2�ü�
//flag��ʾ����code[]���Ƿ���1
int DisplayLine(Codepoint pt1, Codepoint pt2) {
    int flag = 0, i;
    for (i = 0; i < 3; i++) {
        //�ж�code���Ƿ���1
        if (pt1.code[i] == '1' || pt2.code[i] == '1')
            flag = 1;
    }

    //ͨ��flag��3����������ж�
    if (flag == 0)
        //flag==0��code�ﲻ��1����ֱ����0000�ڲ���ȫ��������
        return 0;
    else {
        for (i = 0; i < 3; i++) {
            //����ͬ����λΪ1���򲻾���0000��������
            if ((pt1.code[i] == pt2.code[i]) && pt1.code[i] == '1')
                return 1;
        }
        //code����1��������0000����Ҫ�ü���
        return 2;
    }
}
//�������˵�
//���ǽ�pt1����Ϊֱ����0000�Ľ���
PixelPoint SetEndpt(Codepoint pt1, Codepoint pt2, int x_max, int x_min, int y_max, int y_min) {
    PixelPoint pt;
    Codepoint temp;//ֱ����0000�Ľ���
    int x, y;//xΪֱ�����������0000�߽�����x�����ϵ����꣬yΪy�����ϵ�
    float k, m;//kΪֱ��б�ʣ�mΪֱ����0000���������
    //�ü��������ߵ��߶�
    if (pt1.code[3] == '1')
        x = x_min;//pt1�����
    if (pt1.code[2] == '1')
        x = x_max;//pt1���ұ�
    if (pt1.code[3] == '1' || pt1.code[2] == '1') {
        //pt1����0000��
        k = (float)(pt1.y - pt2.y) / (pt1.x - pt2.x);
        m = (float)pt1.y + k * (x - pt1.x);
        temp.x = x;
        temp.y = m;
        if (temp.y<y_max && temp.y>y_min) {
            //�µ��y����������0000�ڣ��ü�����
            pt.x = temp.x;
            pt.y = temp.y;
            return pt;
        }
    }
    //�ü��������ߵ��߶�
    if (pt1.code[0] == '1')
        y = y_min;//pt1�ڵײ�
    if (pt1.code[1] == '1')
        y = y_max;//pt1���ϲ�
    if (pt1.code[0] == '1' || pt1.code[1] == '1') {
        k = (float)(pt1.y - pt2.y) / (pt1.x - pt2.x);
        m = (float)pt1.x + (y - pt1.y) / k;
        temp.x = m;
        temp.y = y;
        for (int i = 0; i < 3; i++) {
            temp.code[i] = pt1.code[i];
        }
        //�ٲü�һ��
        pt.x = temp.x;
        pt.y = temp.y;
        return pt;
    }
    else
    {
        //pt1��0000�ڲ���ֱ�ӷ���
        pt.x = pt1.x;
        pt.y = pt1.y;
        return pt;
    }
}
void Raster::CohenSutherland(PixelPoint pt1, PixelPoint pt2, Layer* container) {
    //ȷ������0000��Χ
    int x_max = pt1.x > pt2.x ? pt1.x : pt2.x;
    int x_min = pt1.x < pt2.x ? pt1.x : pt2.x;
    int y_max = pt1.y > pt2.y ? pt1.y : pt2.y;
    int y_min = pt1.y < pt2.y ? pt1.y : pt2.y;
    vector<Geometry*> TailorGeo;
    int i = 0;
    while (i < container->getGeometryCount()) {
        //����ѭ����ȷ��ÿһ���ߣ������б���
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
            //�õ��ߵ������˵�����
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
            case 0://����
                nPolylineGeometry->addPoint(Lpt1.x, Lpt1.y);
                nPolylineGeometry->addPoint(Lpt2.x, Lpt2.y);
                break;
            case 1://����
                break;
            case 2://�ü�
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


//�ж��߶ζ˵��ڲü��˿ھ��ε����滹������
int LptOUTERrect(PixelPoint Lpt, PixelPoint rect[2]) {
    //ȷ����Χ
    int x_min, x_max, y_min, y_max;
    x_min = rect[0].x < rect[1].x ? rect[0].x : rect[1].x;
    x_max = rect[0].x > rect[1].x ? rect[0].x : rect[1].x;
    y_min = rect[0].y < rect[1].y ? rect[0].y : rect[1].y;
    y_max = rect[0].y > rect[1].y ? rect[0].y : rect[1].y;
    if (Lpt.x > x_min && Lpt.x<x_max && Lpt.y>y_min && Lpt.y < y_max)
        return 0;
    return 1;
}
//���ֱƽ�������ȷ��ֱ�߶κʹ��ڵĽ���
PixelPoint BSearch_findIntersection(PixelPoint outsidePt, PixelPoint insidePt, PixelPoint* rect) {

    PixelPoint mid; //�����е�����
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
//�ж�ֱ����Խ����Ƿ�ƽ�У���ƽ���򷵻�1���޽���
//����������ˣ�(x1,y1)��(x2,y2)=x1*y2-x2*y1>0������˳ʱ�뷽��<0��������ʱ�뷽��
int judgeParallel(PixelPoint Lpt1, PixelPoint Lpt2, PixelPoint DiagonalPt1, PixelPoint DiagonalPt2) {
    int flag1, flag2;
    //���ж��߶������˵��ڶԽ��ߵ����
    int d1 = DiagonalPt1.x * Lpt1.y - DiagonalPt1.y * Lpt1.x;
    int d2 = DiagonalPt1.x * Lpt2.y - DiagonalPt1.y * Lpt2.x;
    if (d1 * d2 <= 0) {
        //��ƽ�У��ཻ
        flag1 = 0;
    }
    flag1 = 1;
    //���ж϶Խ��������˵����߶ε����
    int d3 = Lpt1.x * DiagonalPt1.y - Lpt1.y * DiagonalPt1.x;
    int d4 = Lpt1.x * DiagonalPt2.y - Lpt1.y * DiagonalPt2.x;
    if (d3 * d4 <= 0) {
        flag2 = 0;
    }
    flag2 = 1;
    if (flag1 == 0 && flag2 == 0)
        return 0;//��ƽ�У��ཻ������0
    else
        return 1;
}
//���߶���Խ��ߵĽ���
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
//�е�ü��㷨
void Raster::MidPointClipAlgorithm(PixelPoint pt1, PixelPoint pt2, Layer* container) {
    //ȷ���ü��˿ھ���
    PixelPoint rect[2];
    rect[0].x = pt1.x < pt2.x ? pt1.x : pt2.x;
    rect[0].y = pt1.y < pt2.y ? pt1.y : pt2.y;
    rect[1].x = pt1.x > pt2.x ? pt1.x : pt2.x;
    rect[1].y = pt1.y > pt2.y ? pt1.y : pt2.y;
    vector<Geometry*> TailorGeo;
    int i = 0;
    //����ͼ��
    while (i < container->getGeometryCount()) {
        //�߶ε������˵�Lpt1,Lpt2
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
            //�߶������˵㶼�ھ�����
            inn1 = Lpt1;
            inn2 = Lpt2;
        }
        if ((codeLpt1 == 1 && codeLpt2 == 0) || (codeLpt1 == 0 && codeLpt2 == 1)) {
            //�߶�ֻ��һ���˵��ھ�����
            if (codeLpt1) {
                //Lpt1���ⲿ
                inn1 = Lpt2;
                out1 = Lpt1;

            }
            else {
                //Lpt1���ڲ�
                inn1 = Lpt1;
                out1 = Lpt2;
            }
            inn2 = BSearch_findIntersection(out1, inn1, rect);
        }
        if (codeLpt1 == 1 && codeLpt2 == 1) {
            //�߶ζ˵㶼�ھ�����
            out1 = Lpt1;
            out2 = Lpt2;
            inn1 = BSearch_findIntersection(out1, out2, rect);
            inn2 = BSearch_findIntersection(out2, out1, rect);
            //���þ��ζԽ������߶εĽ���diagonal_pt�����߶ε��е�mid
            //������ε��ĸ�����abcd����Խ���Ϊad��bc
            PixelPoint a, b, c, d;
            a.x = rect[0].x, a.y = rect[1].y;
            b = rect[0];
            c = rect[1];
            d.x = rect[1].x, d.y = rect[0].y;
            int codeDiagonal1, codeDiagonal2;
            codeDiagonal1 = judgeParallel(out1, out2, a, d);
            codeDiagonal2 = judgeParallel(out1, out2, b, c);
            if (codeDiagonal1 == 0 || codeDiagonal2 == 0) {
                //�ཻ���󽻵�
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
                //�������������ԭ�����߶����߿�û�н��㣬���Խ�inn������Ϊ��
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
//����εĲü�
//����һ���µĵ����
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
//�ٶ���һ����������
struct VectorLine :point
{
    VectorLine(point start, point end)
    {
        this->start = start;
        this->end = end;
    }
    point start;   //��ʼ����
    point end; //�ս��
};
double Multi(point pt, point v_start, point v_end) {
    int x1 = pt.x - v_start.x;
    int y1 = pt.y - v_start.y;
    int x2 = v_end.x - v_start.x;
    int y2 = v_end.y - v_start.y;
    int km = x1 * y2 - x2 * y1;
    //P(x1,y1),Q(x2,y2)
    //P��Q>0,P��Q��˳ʱ�뷽��
    //P��Q<0,P��Q����ʱ�뷽��
    return km;
}
//���Ƿ��ڱ߽����ڲ�
int isInside(point& pt, VectorLine& v) {
    int k = Multi(pt, v.start, v.end) >= 0 ? 0 : 1;
    //���ڲü�������������ʱ��ģ���༴�ھ����ڲ�����Multi>0���ڲ࣬����1
    return k;
}
//��߽������αߵĽ���
point IntersectionPt(point startPt1, point endPt1, point startPt2, point endPt2) {
    double pX = (Multi(startPt1, endPt2, startPt2) * endPt1.x - Multi(endPt1, endPt2, startPt2) * startPt1.x) /
        (Multi(startPt1, endPt2, startPt2) - Multi(endPt1, endPt2, startPt2));
    double pY = (Multi(startPt1, endPt2, startPt2) * endPt1.y - Multi(endPt1, endPt2, startPt2) * startPt1.y) /
        (Multi(startPt1, endPt2, startPt2) - Multi(endPt1, endPt2, startPt2));
    return point(pX, pY);
}
//͹����βü��㷨
//rectPt1Ϊ���βü�������ϽǶ���
//rectPt2Ϊ���βü�������½Ƕ���
void Raster::Sutherland_Hodgeman(PixelPoint rectPt1, PixelPoint rectPt2, Layer* container) {
    //ȷ���ü���Χ
    int x_min = rectPt1.x;
    int x_max = rectPt2.x;
    int y_min = rectPt2.y;
    int y_max = rectPt1.y;
    vector<point>points;
    vector<Geometry*> TailorGeo;
    int i = 0;
    //����ͼ��
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
                points.push_back(point(pts[j].x, pts[j].y)); //�������һ��LIST��
        }
        points.push_back(point(pts[0].x, pts[0].y));
        //�ü����α߽��Ϊ��������ʱ��
        vector<VectorLine> vectors;
        vectors.push_back(VectorLine(point(x_max, y_min), point(x_max, y_max)));
        vectors.push_back(VectorLine(point(x_max, y_max), point(x_min, y_max)));
        vectors.push_back(VectorLine(point(x_min, y_max), point(x_min, y_min)));
        vectors.push_back(VectorLine(point(x_min, y_min), point(x_max, y_min)));
        vector<point> result, cur;
        int vectorsSize = vectors.size();
        int pointSize = points.size();
        if (points.size() > 0) {
            //��SΪ����δ洢�����һ�����㣬��Ϊ�ж��������ʼ��
            //point S = points[points.size()-1];
            point S = points[0];

            for (int j = 0; j < points.size(); ++j)
                result.push_back(points[j]);
            //flag��S�ڱ߽��ڲ࣬�ھͷ���1
            int flag;
            //�����ұ߱߿�ʼ����������
            for (int j = 0; j < vectors.size(); ++j)
            {
                //���ж���ʼ��S�Ƿ��ڱ߽�����
                if (isInside(S, vectors[j]))
                    flag = 1;
                else
                    flag = 0;
                //��ǰ���α�ʱ�����ж���εĵ��Ƿ��ڱߵ��ڲ�
                int resultSize = result.size();
                for (int k = 0; k < resultSize; ++k)
                {
                    //��ǰ�����ڲ�
                    if (isInside(result[k], vectors[j]))
                    {
                        //flag��ʾǰһ���㣬flag==1��ʾ���ڲ�
                        if (flag == 0)
                        {
                            //ǰһ��������࣬��ǰ�����ڲ࣬�󽻵㲢����cur��
                            flag = 1;
                            if (k == 0)
                                cur.push_back(IntersectionPt(S, result[k], vectors[j].start, vectors[j].end));
                            else
                                cur.push_back(IntersectionPt(result[k - 1], result[k], vectors[j].start, vectors[j].end));
                            cur.push_back(result[k]);
                        }
                        else {
                            //�����㶼���ڲֱ࣬�ӱ��浱ǰ�㵽cur��
                            cur.push_back(result[k]);
                        }
                    }
                    else {//��ǰ�������
                        if (flag) {
                            //ǰһ�������ڲ࣬�󽻵㲢����cur��
                            flag = 0;
                            if (k == 0)
                                cur.push_back(IntersectionPt(S, result[k], vectors[j].start, vectors[j].end));
                            else
                                cur.push_back(IntersectionPt(result[k - 1], result[k], vectors[j].start, vectors[j].end));
                        }
                        //�����㶼�����
                        //����ǰ���㸳��S���µ������
                        //S = result[k];
                    }
                }
                //if (j != 0) {
                //	//��cur�������һ��δ��ӵ�S��
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
                    //�����ü�������β������
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