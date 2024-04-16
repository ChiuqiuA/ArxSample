#pragma once
class CPolylineUtil
{
public:
	/**
	  *  @brief: 创建优化多段线
	  *  @param[in] const AcGePoint2dArray & points
	  *  @param[in] double width
	  *  @Return: AcDbObjectId
	  *  @Method: CPolylineUtil::Add
	  */
	static AcDbObjectId Add(const AcGePoint2dArray& points, double width = 0,
		AcDbDatabase* pDb = acdbCurDwg());
	static AcDbObjectId Add(const AcGePoint2d& ptStart, const AcGePoint2d& ptEnd, double width = 0,
		AcDbDatabase* pDb = acdbCurDwg());
};

