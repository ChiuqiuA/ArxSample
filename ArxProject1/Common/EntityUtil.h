#pragma once
class CEntityUtil
{
public:
	// ɾ��һ��ʵ��
	static void Erase(AcDbObjectId entId);

	template <class Ty>
	static void Erase(Ty ids)
	{
		for (const auto& id : ids)
		{
			Erase(id);
		}
	}
};

