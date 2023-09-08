#include "0_light.h"
#include <algorithm>
#include <assert.h>


//���ƽ��
void Getpanel(Vec4& p1, Vec4& p2, Vec4& p3, double& a, double& b, double& c, double& d)
{
    a = ((p2.y() - p1.y()) * (p3.z() - p1.z()) - (p2.z() - p1.z()) * (p3.y() - p1.y()));
    b = ((p2.z() - p1.z()) * (p3.x() - p1.x()) - (p2.x() - p1.x()) * (p3.z() - p1.z()));
    c = ((p2.x() - p1.x()) * (p3.y() - p1.y()) - (p2.y() - p1.y()) * (p3.x() - p1.x()));
    d = (0 - (a * p1.x() + b * p1.y() + c * p1.z()));
}

//z-buffer�㷨������
void Raster1::Zbuffer(Vec4 pts0, Vec4 pts1, Vec4 pts2)
{
    Color color = WHITE;
    Color n_color;
    Vec3 Vector01(pts0.x() - pts1.x(), pts0.y() - pts1.y(), pts0.z() - pts1.z());
    Vec3 Vector02(pts0.x() - pts2.x(), pts0.y() - pts2.y(), pts0.z() - pts2.z());
    Vec3 Normal = Vector01 ^ Vector02;

    Vec3 FragPos;
    Raster1 light;
    Vec4 temp;
    //����y�Ĵ�С��pts0��pts1��pts2��������
    if (pts0.y() > pts1.y())
    {
        temp = pts1;
        pts1 = pts0;
        pts0 = temp;
    }
    if (pts0.y() > pts2.y())
    {
        temp = pts2;
        pts2 = pts0;
        pts0 = temp;
    }
    if (pts1.y() > pts2.y())
    {
        temp = pts2;
        pts2 = pts1;
        pts1 = temp;
    }
    //����x-yƽ���z-yƽ�������������ߵ�б��
    double k0, k1, k2, k00, k11, k22;
    k0 = (pts1.y() - pts0.y()) / (pts1.x() - pts0.x());
    k1 = (pts2.y() - pts0.y()) / (pts2.x() - pts0.x());
    k2 = (pts2.y() - pts1.y()) / (pts2.x() - pts1.x());
    k00 = (pts1.y() - pts0.y()) / (pts1.z() - pts0.z());
    k11 = (pts2.y() - pts0.y()) / (pts2.z() - pts0.z());
    k22 = (pts2.y() - pts1.y()) / (pts2.z() - pts1.z());

    //��ɨ���ߺ�pts0pts1��pts0pts2�Ľ��㣬����y=kx+c��y=kz+c���
    for (int i = pts0.y(); i < (int)pts1.y(); i++)
    {

        Vec4 c0, c1;
        if (pts1.x() == pts0.x())
            c0.x() = pts0.x();
        else
            c0.x() = (i - pts0.y()) / k0 + pts0.x();
        if (pts2.x() == pts0.x())
            c1.x() = pts0.x();
        else
            c1.x() = (i - pts0.y()) / k1 + pts0.x();//y=kx+c
        if (pts1.z() == pts0.z())
            c0.z() = pts0.z();
        else
            c0.z() = (i - pts0.y()) / k00 + pts0.z();
        if (pts2.z() == pts0.z())
            c1.z() = pts0.z();
        else
            c1.z() = (i - pts0.y()) / k11 + pts0.z();//y=kz+c
        //��������x��z�ϵĲ���
        double stepx, stepz;
        stepx = abs(c1.x() - c0.x());
        stepz = abs(c1.z() - c0.z());
        double maxx = max(c0.x(), c1.x());
        double minx = min(c0.x(), c1.x());
        //���ò���stepx�����Z��������setPixel����ͼ��
        for (int j = minx; j < (int)maxx; j++)
        {
            double Z;
            Z = (j - (int)minx) * stepz / stepx;
            FragPos[0] = j;
            FragPos[1] = i;
            FragPos[0] = Z;
            Vec3 I = light.calculateLighting(Normal, FragPos);
            Byte _r = _R(color) * I[0] * 255;
            Byte _g = _G(color) * I[1] * 255;
            Byte _b = _B(color) * I[2] * 255;
            n_color = _RGB(_r, _g, _b);
            setPixel(j, i, Z, n_color);
        }
    }
    //��ɨ���ߺ�pts0pts2��pts1pts2�Ľ��㣬����y=kx+c��y=kz+c���
    for (int i = pts1.y(); i < (int)pts2.y(); i++)
    {
        Vec4 c2, c3;
        if (pts2.x() == pts0.x())
            c2.x() = pts0.x();
        else
            c2.x() = (i - pts0.y()) / k1 + pts0.x();
        if (pts2.x() == pts1.x())
            c3.x() = pts1.x();
        else
            c3.x() = (i - pts1.y()) / k2 + pts1.x();
        if (pts2.z() == pts0.z())
            c2.z() = pts0.z();
        else
            c2.z() = (i - pts0.y()) / k11 + pts0.z();
        if (pts2.z() == pts1.z())
            c3.z() = pts1.z();
        else
            c3.z() = (i - pts1.y()) / k22 + pts1.z();
        //��������x��z�ϵĲ���
        double stepx, stepz;
        stepx = abs(c3.x() - c2.x());
        stepz = abs(c3.z() - c2.z());
        double maxx = max(c2.x(), c3.x());
        double minx = min(c2.x(), c3.x());
        //���ò���stepx�����Z��������setPixel����ͼ��
        for (int j = minx; j < (int)maxx; j++)
        {
            double Z;
            Z = (j - (int)minx) * stepz / stepx;
            FragPos[0] = j;
            FragPos[1] = i;
            FragPos[0] = Z;
            Vec3 I = light.calculateLighting(Normal, FragPos);
            Byte _r = _R(color) * I[0] * 255;
            Byte _g = _G(color) * I[1] * 255;
            Byte _b = _B(color) * I[2] * 255;
            n_color = _RGB(_r, _g, _b);
            setPixel(j, i, Z, n_color);
        }
    }
}
Vec3 lightPos(0, 220, 0);
Vec3 viewPos(0, 0, 0);
Vec3 lightColor(255, 255, 255);
//�����ӵ㡢��ɫ�����λ��

//�����ǿ
Vec3 Raster1::calculateLighting(Vec3& Normal, Vec3& FragPos)
{
    // ambient ������
    float ambientStrength = 0.1;
    Vec3 ambient = lightColor;
    ambient *= ambientStrength;

    // diffuse ���䷴���
    Vec3 norm = Normal; //������
    Vec3 lightDir = lightPos - FragPos; //������
    norm.normalize();
    lightDir.normalize();
    float diff = std::max((norm * lightDir), 0.0);
    Vec3 diffuse = lightColor;
    diffuse *= diff;

    // specular �����
    float specularStrength = 0.5;
    Vec3 viewDir = viewPos - FragPos;
    viewDir.normalize();
    Vec3 reflectDir;
    reflectDir.reflect(lightDir, norm);
    float spec = pow(std::max((viewDir * reflectDir), 0.0), 32);
    //Vec3 specular = specularStrength * spec * lightColor;
    Vec3 specular = lightColor;
    specular *= spec;
    Vec3 light = (diffuse + ambient + specular);

    return light;
}

void Raster1::Zbuffer(Vertex3D pixelSet[])
{
    int count = 3;

    //������ɨ����yֵ
    int maxy = 0;
    for (int i = 0; i < count; i++)
    {
        if (pixelSet[i].pos.y() > maxy)
            maxy = pixelSet[i].pos.y();
    }

    vector<double> crossPointX(count);

    double sx, sy, sz, tx, ty, tz;
    double x;
    double z_max = -99999999;

    for (int i = 0; i < maxy; i++) //ɨ���� y
    {
        crossPointX.clear();
        for (int j = 0; j < count; j++) //x
        {
            sx = pixelSet[j].pos.x();
            sy = pixelSet[j].pos.y();
            tx = pixelSet[(j + 1) % count].pos.x();
            ty = pixelSet[(j + 1) % count].pos.y();

            //�Ƚ�ǰһ����yֵ�뵱ǰ��yֵ
            int lowy = (sy < ty) ? sy : ty;
            int highy = (sy > ty) ? sy : ty;

            //��ɨ���������αߵĽ���ֵ
            if (i < lowy || i >= highy) continue; //û�н�������
            x = sx + (i - sy) * (tx - sx) / (ty - sy);

            double A, B, C, D;
            Getpanel(pixelSet[0].pos, pixelSet[1].pos, pixelSet[2].pos, A, B, C, D);
            float z;
            if (C != 0)
            {
                z = -(A * x + B * i + D) / C;
            }
            else
            {
                continue;
            }
            if (z > z_max) z_max = z;
            //lp.x = x;
            crossPointX.push_back(x);

        }

        //����
        sort(crossPointX.begin(), crossPointX.end());
        //������ԣ�������ɫ
        for (int k = 0, c = crossPointX.size(); k < c; k += 2)
        {
            for (double xx = crossPointX[k]; xx <= crossPointX[(k + 1) % c]; xx++)
            {
                setPixel(int(xx), i, z_max, BLUE);
            }
        }
    }
}