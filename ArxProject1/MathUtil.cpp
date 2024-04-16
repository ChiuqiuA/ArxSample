#include "stdafx.h"
#include "MathUtil.h"

double CMathUtil::PI()
{
	return atan(1.0) * 4;
}

// 判断两个数是否相等
bool CMathUtil::IsEqual(double a, double b, double tol /*= 1.0E-7*/)
{
	return (fabs(a - b) < tol);
}

// 判断两个浮点数是否大于
bool CMathUtil::IsMore(double a, double b, double tol /*= 1.0E-7*/)
{
	return ((a - b) > tol);
}

// 判断两个浮点数是否小于
bool CMathUtil::IsLess(double a, double b, double tol /*= 1.0E-7*/)
{
	return ((a - b) < -tol);
}

// 判断两个浮点数是否大于等于
bool CMathUtil::IsMoreEqual(double a, double b, double tol /*= 1.0E-7*/)
{
	return ((a - b) >= -tol);
}

// 判断两个浮点数是否小于等于
bool CMathUtil::IsLessEqual(double a, double b, double tol /*= 1.0E-7*/)
{
	return ((a - b) <= tol);
}
