#include "StdAfx.h"

struct PointInfo
{
	PointInfo() = default;
	PointInfo(const PointInfo&) = default;
	PointInfo& operator=(const PointInfo&) = default;

	PointInfo(const AcDbObjectId& id, const AcGePoint2dArray& ptArray)
		:m_id(id), m_ptArray(ptArray) {}

	AcDbObjectId m_id;
	AcGePoint2dArray m_ptArray;
};

// �ж��������Ƿ����
bool RealIsEqual(double a, double b, double tol = 1.0E-7)
{
	return (fabs(a - b) < tol);
}

// �ж������������Ƿ�С��
bool RealIsLess(double a, double b, double tol = 1.0E-7)
{
	return ((a - b) < -tol);
}

struct AcGePoint3d_Less
{
	bool operator()(const AcGePoint3d& pt1, const AcGePoint3d& pt2) const
	{
		if (RealIsLess(pt1.x, pt2.x))
		{
			return true;
		}
		else if (RealIsEqual(pt1.x, pt2.x))
		{
			if (RealIsLess(pt1.y, pt2.y))
			{
				return true;
			}
			else if (RealIsEqual(pt1.y, pt2.y))
			{
				if (RealIsLess(pt1.z, pt2.z))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
};

AcGePoint3d ToPoint3d(const AcGePoint2d& point2d, double z = 0)
{
	return AcGePoint3d(point2d.x, point2d.y, z);
}

AcDbObjectId PostToModelSpace(AcDbEntity* pEnt, AcDbDatabase* pDb = acdbCurDwg())
{
	// ��������������Ч��
	std::unique_ptr<AcDbEntity> ptrEnt(pEnt);
	pEnt = nullptr;
	if (!ptrEnt)
	{
		assert(ptrEnt); // ��Ч��assert (pEnt != NULL);
		return AcDbObjectId::kNull;
	}

	// ��ö�Ӧ�Ŀ���¼
	AcDbObjectPointer<AcDbBlockTableRecord> pBTR(acdbSymUtil()->blockModelSpaceId(pDb), AcDb::kForWrite);
	if (!pBTR)
	{
		return AcDbObjectId::kNull;
	}

	// ��ʵ����ӵ�ģ�Ϳռ�Ŀ���¼
	AcDbObjectId	  entId;
	Acad::ErrorStatus es = pBTR->appendAcDbEntity(entId, ptrEnt.get());
	if (es != Acad::eOk)
	{
		return AcDbObjectId::kNull;
	}

	// �ر�ģ�Ϳռ����¼��ʵ��
	ptrEnt->close();
	ptrEnt.release(); // ���뵽����¼����delete��ʵ��

	return entId;
}

AcDbObjectId AddPolyline(const AcGePoint2dArray& points, double width = 0,
	AcDbDatabase* pDb = acdbCurDwg())
{
	int numVertices = points.length();
	AcDbPolyline* pPoly = new AcDbPolyline(numVertices);

	for (int i = 0; i < numVertices; i++)
	{
		pPoly->addVertexAt(i, points.at(i), 0, width, width);
	}

	AcDbObjectId polyId;
	polyId = PostToModelSpace(pPoly, pDb);

	return polyId;
}

void EraseEntity(AcDbObjectId entId)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, entId, AcDb::kForWrite) == Acad::eOk)
	{
		pEnt->erase();
		pEnt->close();
	}
}

template <class Ty>
static void EraseEntities(Ty ids)
{
	for (const auto& id : ids)
	{
		EraseEntity(id);
	}
}

AcDbObjectIdArray MergePolyline(const AcDbObjectIdArray& idArray, bool bRetainMergedMembers = true)
{
	AcDbObjectIdArray idsPline;
	std::multimap<AcGePoint3d, PointInfo, AcGePoint3d_Less> mmapPt2PointInfo;
	for (const auto& id : idArray)
	{
		AcDbObjectPointer<AcDbPolyline> pPline(id);
		if (pPline)
		{
			if (pPline->isClosed())
			{
				idsPline.append(id);
				continue;
			}

			AcGePoint3d ptStart, ptEnd;
			pPline->getStartPoint(ptStart);
			pPline->getEndPoint(ptEnd);

			AcGePoint2dArray ptArray;
			for (uint nIndex = 0; nIndex < pPline->numVerts(); nIndex++)
			{
				AcGePoint2d ptVertex;
				pPline->getPointAt(nIndex, ptVertex);
				ptArray.append(ptVertex);
			}

			mmapPt2PointInfo.emplace(ptStart, PointInfo(id, ptArray));
			mmapPt2PointInfo.emplace(ptEnd, PointInfo(id, ptArray.reverse()));
		}
	}
	if (!mmapPt2PointInfo.size())
	{
		return idsPline;
	}

	while (mmapPt2PointInfo.size())
	{
		AcGePoint2dArray ptArray;
		std::set<AcDbObjectId> setId;
		auto iterFind = mmapPt2PointInfo.begin();
		for (int iCount = 2; iCount > 0 && mmapPt2PointInfo.size(); --iCount)
		{
			// ���������һ�ߵĶ����
			ptArray.reverse();

			if (ptArray.length())
			{
				iterFind = mmapPt2PointInfo.find(ToPoint3d(ptArray.last()));
			}
			while (iterFind != mmapPt2PointInfo.end())
			{
				if (setId.find(iterFind->second.m_id) == setId.end())
				{
					ptArray.append(iterFind->second.m_ptArray);
				}

				setId.insert(iterFind->second.m_id);
				mmapPt2PointInfo.erase(iterFind);
				iterFind = mmapPt2PointInfo.find(ToPoint3d(ptArray.last()));
			}

			// ɾ���Ѿ���ӵ���߶�
			for (iterFind = mmapPt2PointInfo.begin(); iterFind != mmapPt2PointInfo.end(); )
			{
				if (setId.find(iterFind->second.m_id) != setId.end())
				{
					iterFind = mmapPt2PointInfo.erase(iterFind);
				}
				else
				{
					iterFind++;
				}
			}
		}

		if (setId.size() == 1)
		{
			// δ�ϲ��Ķ����Ҳ��Ҫ����
			idsPline.append(*setId.begin());
		}
		else
		{
			AcDbObjectId idPline = AddPolyline(ptArray);
			if (!idPline.isNull())
			{
				idsPline.append(idPline);
			}
			if (ptArray.first() == ptArray.last())
			{
				AcDbObjectPointer<AcDbPolyline> pPline(idPline, AcDb::kForWrite);
				if (pPline)
				{
					pPline->setClosed(true);
				}
			}
			if (!bRetainMergedMembers)
			{
				EraseEntities(setId);
			}
		}
	}

	return idsPline;
}
