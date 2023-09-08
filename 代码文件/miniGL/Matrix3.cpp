#define _CRT_SECURE_NO_WARNINGS 1
#include "Matrix3.h"
#include <math.h>
#include "Vec3.h"
#include "0_Vec4.h"
//平移
Matrix3& Matrix3::makeTranslate(double dx, double dy)
{
    makeIdentity();
    _m[2][0] = dx;
    _m[2][1] = dy;
    return *this;
}

//旋转

Matrix3& Matrix3::makeRotate(double rad)
{
    makeIdentity();

    double _cos = cos(rad);
    double _sin = sin(rad);
    _m[0][0] = _cos;
    _m[0][1] = _sin;
    _m[1][0] = -_sin;
    _m[1][1] = _cos;
    return *this;
}
//放缩

Matrix3& Matrix3::makeScale(double sx, double sy)
{
    makeIdentity();

    _m[0][0] = sx;
    _m[1][1] = sy;
    return *this;
}

Matrix3 Matrix3::translate(double dx, double dy)
{
    Matrix3 m;
    m.makeTranslate(dx, dy);
    return m;
}

Matrix3 Matrix3::rotate(double rad)
{
    Matrix3 m;
    m.makeRotate(rad);
    return m;
}

Matrix3 Matrix3::scale(double sx, double sy)
{
    Matrix3 m;
    m.makeScale(sx, sy);
    return m;
}

Matrix3 Matrix3::n_translate(double dx, double dy)
{
    Matrix3 m;
    m.makeTranslate(-dx, -dy);
    return m;
}

//逆矩阵
Matrix3 Matrix3::matrix_inverse(Matrix3 a)//double(*a)[N]
{
	int N = 3;
	using namespace std;
	int i, j, k;
	double max, temp;
	// 定义一个临时矩阵t
	//double t[N][N];
	Matrix3 t;
	// 将a矩阵临时存放在矩阵t[n][n]中
	Matrix3 b;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			t[i][j] = a[i][j];
		}
	}
	// 初始化B矩阵为单位矩阵
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			b[i][j] = (i == j) ? (double)1 : 0;
		}
	}
	// 进行列主消元，找到每一列的主元
	for (i = 0; i < N; i++)
	{
		max = t[i][i];
		// 用于记录每一列中的第几个元素为主元
		k = i;
		// 寻找每一列中的主元元素
		for (j = i + 1; j < N; j++)
		{
			if (fabs(t[j][i]) > fabs(max))
			{
				max = t[j][i];
				k = j;
			}
		}
		//cout<<"the max number is "<<max<<endl;
		// 如果主元所在的行不是第i行，则进行行交换
		if (k != i)
		{
			// 进行行交换
			for (j = 0; j < N; j++)
			{
				temp = t[i][j];
				t[i][j] = t[k][j];
				t[k][j] = temp;
				// 伴随矩阵B也要进行行交换
				temp = b[i][j];
				b[i][j] = b[k][j];
				b[k][j] = temp;
			}
		}
		if (t[i][i] == 0)
		{
			//cout << "\nthe matrix does not exist inverse matrix\n";
			break;
		}
		// 获取列主元素
		temp = t[i][i];
		// 将主元所在的行进行单位化处理
		//cout<<"\nthe temp is "<<temp<<endl;
		for (j = 0; j < N; j++)
		{
			t[i][j] = t[i][j] / temp;
			b[i][j] = b[i][j] / temp;
		}
		for (j = 0; j < N; j++)
		{
			if (j != i)
			{
				temp = t[j][i];
				//消去该列的其他元素
				for (k = 0; k < N; k++)
				{
					t[j][k] = t[j][k] - temp * t[i][k];
					b[j][k] = b[j][k] - temp * b[i][k];
				}
			}

		}

	}
	return b;
}

//使用高斯消元法对矩阵进行求逆
Matrix3 Matrix3::Gaussian_elimination(Matrix3 a)
{
	int N = 3;
	int i, j, k;
	float W[3][2 * 3];// , result[3][3];
	float tem_1, tem_2, tem_3;
	Matrix3 result;
	// 对矩阵右半部分进行扩增
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 2 * 3; j++) {
			if (j < 3) {
				W[i][j] = (float)a[i][j];
			}
			else {
				W[i][j] = (float)(j - 3 == i ? 1 : 0);
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		// 判断矩阵第一行第一列的元素是否为0，若为0，继续判断第二行第一列元素，直到不为0，将其加到第一行
		if (((int)W[i][i]) == 0)
		{
			for (j = i + 1; j < N; j++)
			{
				if (((int)W[j][i]) != 0) break;
			}
			if (j == N)
			{
				
				break;
			}
			//将前面为0的行加上后面某一行
			for (k = 0; k < 2 * N; k++)
			{
				W[i][k] += W[j][k];
			}
		}

		//将前面行首位元素置1
		tem_1 = W[i][i];
		for (j = 0; j < 2 * N; j++)
		{
			W[i][j] = W[i][j] / tem_1;
		}

		//将后面所有行首位元素置为0
		for (j = i + 1; j < N; j++)
		{
			tem_2 = W[j][i];
			for (k = i; k < 2 * N; k++)
			{
				W[j][k] = W[j][k] - tem_2 * W[i][k];
			}
		}
	}

	// 将矩阵前半部分标准化
	for (i = N - 1; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			tem_3 = W[j][i];
			for (k = i; k < 2 * N; k++)
			{
				W[j][k] = W[j][k] - tem_3 * W[i][k];
			}
		}
	}

	//得出逆矩阵
	for (i = 0; i < N; i++)
	{
		for (j = N; j < 2 * N; j++)
		{
			result[i][j - N] = W[i][j];
		}
	}

	return result;


}
//向量和矩阵坐标系进行变换
Vec4 vectorMultiplyMatrix3D(Vec4& vec, Matrix4& m)
{
	Vec4 result;

	result.x() = vec.x() * m[0][0] + vec.y() * m[1][0] + vec.z() * m[2][0] + vec.w() * m[3][0];
	result.y() = vec.x() * m[0][1] + vec.y() * m[1][1] + vec.z() * m[2][1] + vec.w()* m[3][1];
	result.z() = vec.x() * m[0][2] + vec.y() * m[1][2] + vec.z() * m[2][2] + vec.w() * m[3][2];
	result.w() = vec.x() * m[0][3] + vec.y() * m[1][3] + vec.z() * m[2][3] + vec.w() * m[3][3];

	return result;
}

//计算固定值
double Matrix3::Cal_Determinant()
{
	double M3Val = 0;//行列式的值
	M3Val = _m[0][0] * _m[1][1] * _m[2][2] +
		_m[0][1] * _m[1][2] * _m[2][0] +
		_m[0][2] * _m[1][0] * _m[2][1] -
		_m[2][0] * _m[1][1] * _m[0][2] -
		_m[0][0] * _m[2][1] * _m[1][2] -
		_m[2][2] * _m[1][0] * _m[0][1];
	return M3Val;
}