#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>

int main(){

	Vector3d v(2, 1, 1);
	Matrix3d m, n;

	m << 1 / sqrt(2), -1 / sqrt(2), 0,
		1 / sqrt(2), 1 / sqrt(2), 0,
		0, 0, 1;//��ת������ʱ��45��

	n << 1, 0, 1,
		0, 1, 2,
		0, 0, 1;//ƽ�ƾ���ƽ�� (1,2)

	cout << n * m * v << endl;//����ת��ƽ��

    return 0;
}