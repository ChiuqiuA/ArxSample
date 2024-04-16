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
};

