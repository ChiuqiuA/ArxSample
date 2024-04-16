#include "stdafx.h"
#include "EntityUtil.h"

void CEntityUtil::Erase(AcDbObjectId entId)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, entId, AcDb::kForWrite) == Acad::eOk)
	{
		pEnt->erase();
		pEnt->close();
	}
}
