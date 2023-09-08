#pragma once
//变换矩阵,初始为单位矩阵
struct Matrix3 
{
	double _m[3][3];

	Matrix3() 
	{
		makeIdentity();
	}
	void makeIdentity() 
	{
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				_m[i][j] = 0.0;
		for (int i = 0; i <= 2; i++) {
			_m[i][i] = 1.0;
		}
	}

	double* operator[](int i)
	{
		return _m[i];
	}

	Matrix3& makeTranslate(double dx, double dy);
	Matrix3& makeRotate(double rad);
	Matrix3& makeScale(double sx, double sy);

	static Matrix3 translate(double dx, double dy);
	static Matrix3 rotate(double rad);
	static Matrix3 scale(double sx, double sy);

	//逆变换
	static Matrix3 n_translate(double dx, double dy);
	//逆矩阵
	static Matrix3 matrix_inverse(Matrix3 a);
	static Matrix3 Gaussian_elimination(Matrix3 a);
	double Cal_Determinant();
	//重载*
	Matrix3 operator* (Matrix3& other)
	{
		Matrix3 m;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				m._m[i][j] = 0;
				for (int k = 0; k < 3; k++) {
					m._m[i][j] += _m[i][k] * other._m[k][j];
					/*一共有3个Matrix*/
				}
			}
		}
		return m;
	}
	//重载*=
	Matrix3& operator*= (Matrix3& other)
	{
		Matrix3 m = *this * other;
		*this = m;
		return *this;
	}


	Matrix3 makeInverseMatrix() {
		int n = 3;
		double detVal = Cal_Determinant();
		Matrix3 partner;//伴随矩阵
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				double p[2][2];
				int curRow = 0, curColumn = 0;
				/*生成去除所在行列的矩阵*/
				for (int a = 0; a < n; a++)
				{
					if (a == i) continue;
					for (int b = 0; b < n; b++)
					{
						if (b == j) continue;
						p[curRow][curColumn] = _m[a][b];
						++curColumn;
					}
					curRow++;
					curColumn = 0;
				}
				
				partner[j][i] = (p[0][0] * p[1][1] - p[0][1] * p[1][0]) * ((i + j) % 2 == 0 ? 1 : -1) / detVal;
			}
		}
		return partner;
	}
};

