#include "stdafx.h"
#include "Tool.h"

CString GetNextFilePath(const CString& csFilePathWithoutExt, const CString& csExt, int nIndex, BOOL bOverride /*= FALSE*/)
{
    CString csPath;
    if (nIndex <= 0)
    {
        csPath = csFilePathWithoutExt + csExt;
    }
    else
    {
        csPath = csFilePathWithoutExt;
        csPath.AppendFormat(_T("_%d") + csExt, nIndex);
    }

    if (!bOverride)
    {
        while (PathFileExists(csPath))
        {
            csPath = csFilePathWithoutExt;
            csPath.AppendFormat(_T("_%d") + csExt, ++nIndex);
        }
    }

    return csPath;
}
