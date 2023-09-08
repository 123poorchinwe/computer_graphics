#define _CRT_SECURE_NO_WARNINGS 1
#include "Matrix3.h"
#include <math.h>
#include "Vec3.h"
#include "0_Vec4.h"
//ƽ��
Matrix3& Matrix3::makeTranslate(double dx, double dy)
{
    makeIdentity();
    _m[2][0] = dx;
    _m[2][1] = dy;
    return *this;
}

//��ת

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
//����

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

//�����
Matrix3 Matrix3::matrix_inverse(Matrix3 a)//double(*a)[N]
{
	int N = 3;
	using namespace std;
	int i, j, k;
	double max, temp;
	// ����һ����ʱ����t
	//double t[N][N];
	Matrix3 t;
	// ��a������ʱ����ھ���t[n][n]��
	Matrix3 b;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			t[i][j] = a[i][j];
		}
	}
	// ��ʼ��B����Ϊ��λ����
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			b[i][j] = (i == j) ? (double)1 : 0;
		}
	}
	// ����������Ԫ���ҵ�ÿһ�е���Ԫ
	for (i = 0; i < N; i++)
	{
		max = t[i][i];
		// ���ڼ�¼ÿһ���еĵڼ���Ԫ��Ϊ��Ԫ
		k = i;
		// Ѱ��ÿһ���е���ԪԪ��
		for (j = i + 1; j < N; j++)
		{
			if (fabs(t[j][i]) > fabs(max))
			{
				max = t[j][i];
				k = j;
			}
		}
		//cout<<"the max number is "<<max<<endl;
		// �����Ԫ���ڵ��в��ǵ�i�У�������н���
		if (k != i)
		{
			// �����н���
			for (j = 0; j < N; j++)
			{
				temp = t[i][j];
				t[i][j] = t[k][j];
				t[k][j] = temp;
				// �������BҲҪ�����н���
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
		// ��ȡ����Ԫ��
		temp = t[i][i];
		// ����Ԫ���ڵ��н��е�λ������
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
				//��ȥ���е�����Ԫ��
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

//ʹ�ø�˹��Ԫ���Ծ����������
Matrix3 Matrix3::Gaussian_elimination(Matrix3 a)
{
	int N = 3;
	int i, j, k;
	float W[3][2 * 3];// , result[3][3];
	float tem_1, tem_2, tem_3;
	Matrix3 result;
	// �Ծ����Ұ벿�ֽ�������
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
		// �жϾ����һ�е�һ�е�Ԫ���Ƿ�Ϊ0����Ϊ0�������жϵڶ��е�һ��Ԫ�أ�ֱ����Ϊ0������ӵ���һ��
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
			//��ǰ��Ϊ0���м��Ϻ���ĳһ��
			for (k = 0; k < 2 * N; k++)
			{
				W[i][k] += W[j][k];
			}
		}

		//��ǰ������λԪ����1
		tem_1 = W[i][i];
		for (j = 0; j < 2 * N; j++)
		{
			W[i][j] = W[i][j] / tem_1;
		}

		//��������������λԪ����Ϊ0
		for (j = i + 1; j < N; j++)
		{
			tem_2 = W[j][i];
			for (k = i; k < 2 * N; k++)
			{
				W[j][k] = W[j][k] - tem_2 * W[i][k];
			}
		}
	}

	// ������ǰ�벿�ֱ�׼��
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

	//�ó������
	for (i = 0; i < N; i++)
	{
		for (j = N; j < 2 * N; j++)
		{
			result[i][j - N] = W[i][j];
		}
	}

	return result;


}
//�����;�������ϵ���б任
Vec4 vectorMultiplyMatrix3D(Vec4& vec, Matrix4& m)
{
	Vec4 result;

	result.x() = vec.x() * m[0][0] + vec.y() * m[1][0] + vec.z() * m[2][0] + vec.w() * m[3][0];
	result.y() = vec.x() * m[0][1] + vec.y() * m[1][1] + vec.z() * m[2][1] + vec.w()* m[3][1];
	result.z() = vec.x() * m[0][2] + vec.y() * m[1][2] + vec.z() * m[2][2] + vec.w() * m[3][2];
	result.w() = vec.x() * m[0][3] + vec.y() * m[1][3] + vec.z() * m[2][3] + vec.w() * m[3][3];

	return result;
}

//����̶�ֵ
double Matrix3::Cal_Determinant()
{
	double M3Val = 0;//����ʽ��ֵ
	M3Val = _m[0][0] * _m[1][1] * _m[2][2] +
		_m[0][1] * _m[1][2] * _m[2][0] +
		_m[0][2] * _m[1][0] * _m[2][1] -
		_m[2][0] * _m[1][1] * _m[0][2] -
		_m[0][0] * _m[2][1] * _m[1][2] -
		_m[2][2] * _m[1][0] * _m[0][1];
	return M3Val;
}