#pragma once
class CEntityUtil
{
public:
	// 删除一个实体
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

