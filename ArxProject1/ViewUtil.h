#pragma once
class CViewUtil
{
public:
	// 获得当前的视图设置
	static void GetCurrentView(AcDbViewTableRecord& view);

	// 给定显示范围的最大、最小角点坐标和缩放比例，修改视图
	static void Set(const AcGePoint3d& ptMin, const AcGePoint3d& ptMax, double scale = 1.0);
};

