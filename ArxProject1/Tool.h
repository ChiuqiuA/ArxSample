#pragma once
class CTool
{
};

void GetCurrentView(AcDbViewTableRecord& view);
CString GetNextFilePath(const CString& csFilePathWithoutExt, const CString& csExt, int nIndex, BOOL bOverride = FALSE);
AcGePoint3d WcsToDcsPoint(const AcGePoint3d& point);
void ViewSet(const AcGePoint3d& ptMin, const AcGePoint3d& ptMax, double scale = 1.0);
