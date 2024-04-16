#pragma once
class CMathUtil
{
public:
	/**
  *  @brief: 获得PI的值
  *  @Return: double
  *  @Method: CMathUtil::PI
  */
	static double PI();

	/**
	  *  @brief: 判断两个浮点数是否相等
	  *  @param[in]  double a
	  *  @param[in]  double b
	  *  @param[in]  double tol
	  *  @Return: bool
	  *  @Method: IsEqual
	  */
	static bool IsEqual(double a, double b, double tol = 1.0E-7);

	/**
	  *  @brief: 判断两个浮点数是否大于
	  *  @param[in]  double a
	  *  @param[in]  double b
	  *  @param[in]  double tol
	  *  @Return: bool
	  *  @Method: IsMore
	  */
	static bool IsMore(double a, double b, double tol = 1.0E-7);

	/**
	  *  @brief: 判断两个浮点数是否小于
	  *  @param[in] double a
	  *  @param[in] double b
	  *  @param[in] double tol
	  *  @Return: bool
	  *  @Method: IsLess
	  */
	static bool IsLess(double a, double b, double tol = 1.0E-7);


	/**
	  *  @brief: 判断两个浮点数是否大于等于
	  *  @param[in] double a
	  *  @param[in] double b
	  *  @param[in] double tol
	  *  @Return: bool
	  *  @Method: IsMoreEqual
	  */
	static bool IsMoreEqual(double a, double b, double tol = 1.0E-7);


	/**
	  *  @brief: 判断两个浮点数是否小于等于
	  *  @param[in] double a
	  *  @param[in] double b
	  *  @param[in] double tol
	  *  @Return: bool
	  *  @Method: IsLessEqual
	  */
	static bool IsLessEqual(double a, double b, double tol = 1.0E-7);
};

