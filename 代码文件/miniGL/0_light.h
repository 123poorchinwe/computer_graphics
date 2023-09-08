#include<cmath>
#include <algorithm>
#include "Graphic.h"
#include <time.h>
#include "0_Vec4.h"
#include "Vec3.h"
#include <assert.h>
#include"GeoDefine3D.h"



class Raster1
{
public:
	static void Paint(Vec4 pts0, Vec4 pts1, Vec4 pts2);
	void Zbuffer(Vertex3D pixelSet[]);
	static void Zbuffer(Vec4 pts0, Vec4 pts1, Vec4 pts2);//深度缓存(调用了光照)
	Vec3 calculateLighting(Vec3& Normal, Vec3& FragPos);//phong算法计算光照

};