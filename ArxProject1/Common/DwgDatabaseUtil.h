#pragma once
class CDwgDatabaseUtil
{
public:
	/**
	  *  @brief:  将实体添加到模型空间
	  *  @param[in]
	  *  @param[out]
	  *  @Return: 实体Id
	  *  @Method:  PostToModelSpace
	  */
	static AcDbObjectId PostToModelSpace(AcDbEntity* pEnt, AcDbDatabase* pDb = acdbCurDwg());
};

