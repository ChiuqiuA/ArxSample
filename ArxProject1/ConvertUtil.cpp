#include "stdafx.h"
#include "ConvertUtil.h"

AcGePoint3d CConvertUtil::ToPoint3d(const AcGePoint2d& point2d, double z)
{
	return AcGePoint3d(point2d.x, point2d.y, z);
}

AcGePoint2d CConvertUtil::ToPoint2d(const AcGePoint3d& point3d)
{
	return AcGePoint2d(point3d.x, point3d.y);
}
