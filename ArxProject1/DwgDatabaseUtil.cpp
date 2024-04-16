#include "stdafx.h"
#include "DwgDatabaseUtil.h"

AcDbObjectId CDwgDatabaseUtil::PostToModelSpace(AcDbEntity* pEnt, AcDbDatabase* pDb /*= acdbCurDwg()*/)
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
