#pragma once
class CViewUtil
{
public:
	// ��õ�ǰ����ͼ����
	static void GetCurrentView(AcDbViewTableRecord& view);

	// ������ʾ��Χ�������С�ǵ���������ű������޸���ͼ
	static void Set(const AcGePoint3d& ptMin, const AcGePoint3d& ptMax, double scale = 1.0);
};

