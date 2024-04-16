#include "stdafx.h"
#include "PolylineUtil.h"
#include "DwgDatabaseUtil.h"

AcDbObjectId CPolylineUtil::Add(const AcGePoint2dArray& points, double width,
	AcDbDatabase* pDb /*= acdbCurDwg()*/)
{
	int numVertices = points.length();
	AcDbPolyline* pPoly = new AcDbPolyline(numVertices);

	for (int i = 0; i < numVertices; i++)
	{
		pPoly->addVertexAt(i, points.at(i), 0, width, width);
	}

	AcDbObjectId polyId;
	polyId = CDwgDatabaseUtil::PostToModelSpace(pPoly, pDb);

	return polyId;
}

AcDbObjectId CPolylineUtil::Add(const AcGePoint2d& ptStart, const AcGePoint2d& ptEnd, double width,
	AcDbDatabase* pDb /*= acdbCurDwg()*/)
{
	AcGePoint2dArray points;
	points.append(ptStart);
	points.append(ptEnd);

	return Add(points, width, pDb);
}
