#pragma once
class CConvertUtil
{
public:
	/**
	  *  @brief:  ��ά��תΪ��ά��
	  *  @param[in] const AcGePoint2d & point2d
	  *  @param[in] double z
	  *  @Return: AcGePoint3d
	  *  @Method: CConvertUtil::ToPoint3d
	  */
	static AcGePoint3d ToPoint3d(const AcGePoint2d& point2d, double z = 0);

	/**
	  *  @brief: ��ά��תΪ��ά��
	  *  @param[in] const AcGePoint3d & point3d
	  *  @Return: AcGePoint2d
	  *  @Method: CConvertUtil::ToPoint2d
	  */
	static AcGePoint2d ToPoint2d(const AcGePoint3d& point3d);

	/**
	  *  @brief:��������ϵ����ת��ʾ����ϵ
	  *  @param[in] const AcGePoint3d & point
	  *  @Return: AcGePoint3d
	  *  @Method: CConvertUtil::WcsToDcsPoint
	  */
	static AcGePoint3d WcsToDcsPoint(const AcGePoint3d& point);

	/**
	  *  @brief: �û�����ϵ����ת��������ϵ
	  *  @param[in] const AcGePoint3d &point
	  *  @Return: void
	  *  @Method:  CConvertUtil::UcsToWcsPoint
	  */
	static AcGePoint3d UcsToWcsPoint(const AcGePoint3d& point);

	/**
	  *  @brief:��������ϵ����ת�û�����ϵ
	  *  @param[in] const AcGePoint3d &point
	  *  @Return: void
	  *  @Method:  CConvertUtil::WcsToUcsPoint
	  */
	static AcGePoint3d WcsToUcsPoint(const AcGePoint3d& point);
};

