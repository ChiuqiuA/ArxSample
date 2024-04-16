#include "stdafx.h"
#include "DwgDatabaseUtil.h"

AcDbObjectId CDwgDatabaseUtil::PostToModelSpace(AcDbEntity* pEnt, AcDbDatabase* pDb /*= acdbCurDwg()*/)
{
	// 检查输入参数的有效性
	std::unique_ptr<AcDbEntity> ptrEnt(pEnt);
	pEnt = nullptr;
	if (!ptrEnt)
	{
		assert(ptrEnt); // 等效于assert (pEnt != NULL);
		return AcDbObjectId::kNull;
	}

	// 获得对应的块表记录
	AcDbObjectPointer<AcDbBlockTableRecord> pBTR(acdbSymUtil()->blockModelSpaceId(pDb), AcDb::kForWrite);
	if (!pBTR)
	{
		return AcDbObjectId::kNull;
	}

	// 将实体添加到模型空间的块表记录
	AcDbObjectId	  entId;
	Acad::ErrorStatus es = pBTR->appendAcDbEntity(entId, ptrEnt.get());
	if (es != Acad::eOk)
	{
		return AcDbObjectId::kNull;
	}

	// 关闭模型空间块表记录和实体
	ptrEnt->close();
	ptrEnt.release(); // 加入到块表记录后不能delete该实体

	return entId;
}
