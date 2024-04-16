#include "StdAfx.h"
#include "Common/Common.h"

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

struct AcGePoint3d_Less
{
	bool operator()(const AcGePoint3d& pt1, const AcGePoint3d& pt2) const
	{
		if (CMathUtil::IsLess(pt1.x, pt2.x))
		{
			return true;
		}
		else if (CMathUtil::IsEqual(pt1.x, pt2.x))
		{
			if (CMathUtil::IsLess(pt1.y, pt2.y))
			{
				return true;
			}
			else if (CMathUtil::IsEqual(pt1.y, pt2.y))
			{
				if (CMathUtil::IsLess(pt1.z, pt2.z))
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
			// 反向添加另一边的多段线
			ptArray.reverse();

			if (ptArray.length())
			{
				iterFind = mmapPt2PointInfo.find(CConvertUtil::ToPoint3d(ptArray.last()));
			}
			while (iterFind != mmapPt2PointInfo.end())
			{
				if (setId.find(iterFind->second.m_id) == setId.end())
				{
					ptArray.append(iterFind->second.m_ptArray);
				}

				setId.insert(iterFind->second.m_id);
				mmapPt2PointInfo.erase(iterFind);
				iterFind = mmapPt2PointInfo.find(CConvertUtil::ToPoint3d(ptArray.last()));
			}

			// 删除已经添加点的线段
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
			// 未合并的多段线也需要返回
			idsPline.append(*setId.begin());
		}
		else
		{
			AcDbObjectId idPline = CPolylineUtil::Add(ptArray);
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
				CEntityUtil::Erase(setId);
			}
		}
	}

	return idsPline;
}
