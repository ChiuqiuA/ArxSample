#pragma once
class CMathUtil
{
public:
	/**
  *  @brief: ���PI��ֵ
  *  @Return: double
  *  @Method: CMathUtil::PI
  */
	static double PI();

	/**
	  *  @brief: �ж������������Ƿ����
	  *  @param[in]  double a
	  *  @param[in]  double b
	  *  @param[in]  double tol
	  *  @Return: bool
	  *  @Method: IsEqual
	  */
	static bool IsEqual(double a, double b, double tol = 1.0E-7);

	/**
	  *  @brief: �ж������������Ƿ����
	  *  @param[in]  double a
	  *  @param[in]  double b
	  *  @param[in]  double tol
	  *  @Return: bool
	  *  @Method: IsMore
	  */
	static bool IsMore(double a, double b, double tol = 1.0E-7);

	/**
	  *  @brief: �ж������������Ƿ�С��
	  *  @param[in] double a
	  *  @param[in] double b
	  *  @param[in] double tol
	  *  @Return: bool
	  *  @Method: IsLess
	  */
	static bool IsLess(double a, double b, double tol = 1.0E-7);


	/**
	  *  @brief: �ж������������Ƿ���ڵ���
	  *  @param[in] double a
	  *  @param[in] double b
	  *  @param[in] double tol
	  *  @Return: bool
	  *  @Method: IsMoreEqual
	  */
	static bool IsMoreEqual(double a, double b, double tol = 1.0E-7);


	/**
	  *  @brief: �ж������������Ƿ�С�ڵ���
	  *  @param[in] double a
	  *  @param[in] double b
	  *  @param[in] double tol
	  *  @Return: bool
	  *  @Method: IsLessEqual
	  */
	static bool IsLessEqual(double a, double b, double tol = 1.0E-7);
};

