#pragma once
class CTool
{
};

CString GetNextFilePath(const CString& csFilePathWithoutExt, const CString& csExt, int nIndex, BOOL bOverride = FALSE);
