#include "stdafx.h"
#include "MathUtil.h"

double CMathUtil::PI()
{
	return atan(1.0) * 4;
}

// �ж��������Ƿ����
bool CMathUtil::IsEqual(double a, double b, double tol /*= 1.0E-7*/)
{
	return (fabs(a - b) < tol);
}

// �ж������������Ƿ����
bool CMathUtil::IsMore(double a, double b, double tol /*= 1.0E-7*/)
{
	return ((a - b) > tol);
}

// �ж������������Ƿ�С��
bool CMathUtil::IsLess(double a, double b, double tol /*= 1.0E-7*/)
{
	return ((a - b) < -tol);
}

// �ж������������Ƿ���ڵ���
bool CMathUtil::IsMoreEqual(double a, double b, double tol /*= 1.0E-7*/)
{
	return ((a - b) >= -tol);
}

// �ж������������Ƿ�С�ڵ���
bool CMathUtil::IsLessEqual(double a, double b, double tol /*= 1.0E-7*/)
{
	return ((a - b) <= tol);
}
