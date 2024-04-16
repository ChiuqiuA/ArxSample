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

AcGePoint3d CConvertUtil::WcsToDcsPoint(const AcGePoint3d& point)
{
	// 转换成世界坐标	
	AcGePoint3d pt;
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 0; // from WCS
	rbTo.restype = RTSHORT;
	rbTo.resval.rint = 2; // to DCS

	acedTrans(asDblArray(point), &rbFrom, &rbTo, Adesk::kFalse, asDblArray(pt));

	return pt;
}

AcGePoint3d CConvertUtil::UcsToWcsPoint(const AcGePoint3d& point)
{
	// 转换成世界坐标	
	AcGePoint3d pt;
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 1; // from UCS
	rbTo.restype = RTSHORT;
	rbTo.resval.rint = 0; // to WCS

	acedTrans(asDblArray(point), &rbFrom, &rbTo, Adesk::kFalse, asDblArray(pt));

	return pt;
}

AcGePoint3d CConvertUtil::WcsToUcsPoint(const AcGePoint3d& point)
{
	// 转换成世界坐标	
	AcGePoint3d pt;
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 0; // from WCS
	rbTo.restype = RTSHORT;
	rbTo.resval.rint = 1; // to UCS

	acedTrans(asDblArray(point), &rbFrom, &rbTo, Adesk::kFalse, asDblArray(pt));

	return pt;
}
