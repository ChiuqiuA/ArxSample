#pragma once
class CDwgDatabaseUtil
{
public:
	/**
	  *  @brief:  ��ʵ����ӵ�ģ�Ϳռ�
	  *  @param[in]
	  *  @param[out]
	  *  @Return: ʵ��Id
	  *  @Method:  PostToModelSpace
	  */
	static AcDbObjectId PostToModelSpace(AcDbEntity* pEnt, AcDbDatabase* pDb = acdbCurDwg());
};

