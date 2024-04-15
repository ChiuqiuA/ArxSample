#include "stdafx.h"
#include "BatchPlot.h"
#include <vector>
#include <set>
#include "Tool.h"

struct BlockInfo
{
    std::string			   m_sDevice_Name;
    std::string			   m_sPaper_Setting;
    std::string			   m_sPlot_Setting;

    AcDbExtents m_Extents;
    std::string m_sPlotFileName;
};

struct PlotSettingInfo
{
    // 打印文件信息
    std::vector<BlockInfo> m_vecBlockInfo4Plot;

    // 输出设置
    CString m_csSaveLocation;
    BOOL	m_bOverride = FALSE;

    // 打印图层
    AcDbObjectId m_idLayout;
};

class SystemVar
{
public:
    SystemVar() = delete;
    SystemVar(const SystemVar&) = delete;
    SystemVar& operator=(const SystemVar&) = delete;
    SystemVar(const CString& csSystemVar, short nValue) : m_csSystemVar(csSystemVar)
    {
        // 设置BACKGROUNDPLOT
        acedGetVar(m_csSystemVar, &m_rb);
        if (m_rb.restype == RTSHORT && m_rb.resval.rint != nValue)
        {
            short nOld = m_rb.resval.rint;
            m_rb.resval.rint = nValue;
            acedSetVar(m_csSystemVar, &m_rb);
            m_rb.resval.rint = nOld;
        }
    }

    ~SystemVar()
    {
        // 恢复原BACKGROUNDPLOT
        acedSetVar(m_csSystemVar, &m_rb);
    }

private:
    struct resbuf m_rb {};
    const CString m_csSystemVar;
};

void SetPlotInfo(const BlockInfo& blockInfo, AcDbObjectId& idLayout, AcPlPlotInfo& plotInfo)
{
    Acad::ErrorStatus es;
    AcDbObjectPointer<AcDbLayout> pLayout(idLayout);
    AcDbPlotSettingsValidator* pPSV = acdbHostApplicationServices()->plotSettingsValidator();
    if (!pLayout || !pPSV)
    {
        return;
    }

    auto pLayoutNew = std::make_shared<AcDbPlotSettings>(pLayout->modelType());
    pLayoutNew->copyFrom(pLayout);

    // 打印机名称
    es = pPSV->setPlotCfgName(pLayoutNew.get(), CString(blockInfo.m_sDevice_Name.data()));

    // 设置打印纸张
    es = pPSV->setCanonicalMediaName(pLayoutNew.get(), CString(blockInfo.m_sPaper_Setting.data()));
    //es = pPSV->setClosestMediaName(pLayoutNew.get(), paperWidth, paperHeight, units, matchPrintableArea);    // 设置最合适的打印纸张

    // 设置打印样式表
    es = pPSV->setCurrentStyleSheet(pLayoutNew.get(), CString(blockInfo.m_sPlot_Setting.data()));

    // 打印比例
    es = pPSV->setUseStandardScale(pLayoutNew.get(), true);
    es = pPSV->setStdScaleType(pLayoutNew.get(), AcDbPlotSettings::kScaleToFit);   // 使用枚举值设置标准缩放；或者使用setStdScale根据浮点值设置标准缩放
    //es = pPSV->setCustomPrintScale(pLayoutNew.get(), numerator, denominator);  // 设置自定义打印比例
    //es = pPSV->setPlotPaperUnits(pLayoutNew.get(), units);  // 设置纸张单位：英尺、毫米、像素

    // 打印偏移
    es = pPSV->setPlotCentered(pLayoutNew.get(), true);
    //es = pPSV->setPlotOrigin(pLayoutNew, xCoordinate, yCoordinate);  // 设置打印偏移（原点设置在可打印区域）

    // 设置页面旋转
    es = pPSV->setPlotRotation(pLayoutNew.get(), PlotRotation::k0degrees);

    // 打印范围
    if (blockInfo.m_Extents.minPoint() == AcGePoint3d::kOrigin && blockInfo.m_Extents.maxPoint() == AcGePoint3d::kOrigin)
    {
        es = pPSV->setPlotType(pLayoutNew.get(), AcDbPlotSettings::kExtents);
    }
    else
    {
        AcGePoint3d ptMinDcs = WcsToDcsPoint(blockInfo.m_Extents.minPoint() - AcGeVector3d(1, 1, 0));
        AcGePoint3d ptMaxDcs = WcsToDcsPoint(blockInfo.m_Extents.maxPoint() + AcGeVector3d(1, 1, 0));
        es = pPSV->setPlotWindowArea(pLayoutNew.get(), ptMinDcs.x, ptMinDcs.y, ptMaxDcs.x, ptMaxDcs.y);// 设置打印窗口区域

        es = pPSV->setPlotType(pLayoutNew.get(), AcDbPlotSettings::kWindow);
    }

    // First, set the layout to the specified layout 
    // (which is the current layout in this sample).
    plotInfo.setLayout(idLayout);// This is required.

    // Now, override the layout settings with the plot settings 
    // we have been populating.
    plotInfo.setOverrideSettings(pLayoutNew.get());

    // We need to validate these settings.
    AcPlPlotInfoValidator validator;
    validator.setMediaMatchingPolicy(AcPlPlotInfoValidator::MatchingPolicy::kMatchEnabled);
    es = validator.validate(plotInfo);
}

void DoPlot(AcPlPlotProgressDialog* pPlotProgDlg, AcPlPlotEngine* pEngine, AcPlPlotInfo& plotInfo, bool bPlotToFile, CString csFileName)
{
    Acad::ErrorStatus es = pEngine->beginPlot(pPlotProgDlg);

    // Begin document.  The version we call is dependent 
    // on the plot-to-file status.
    const wchar_t* szDocName = acDocManager->curDocument()->fileName();
    if (bPlotToFile)
        es = pEngine->beginDocument(plotInfo, szDocName, NULL, 1, true, csFileName);
    else
        es = pEngine->beginDocument(plotInfo, szDocName);

    // Follow through sending commands to the engine, 
    // and notifications to the progress dialog.
    pPlotProgDlg->onBeginSheet();
    pPlotProgDlg->setSheetProgressRange(0, 100);
    pPlotProgDlg->setSheetProgressPos(0);

    AcPlPlotPageInfo pageInfo;
    es = pEngine->beginPage(pageInfo, plotInfo, true);
    es = pEngine->beginGenerateGraphics();
    es = pEngine->endGenerateGraphics();
    es = pEngine->endPage();

    pPlotProgDlg->setSheetProgressPos(100);
    pPlotProgDlg->onEndSheet();

    es = pEngine->endDocument();
    es = pEngine->endPlot();
}

void PlotDwgToPdf(PlotSettingInfo& plotSettingInfo)
{
    // 设置系统变量BACKGROUNDPLOT
    SystemVar backgroudplot(_T("BACKGROUNDPLOT"), 0);

    // 获取原视图
    BOOL bNeedRestoreView = FALSE;
    AcDbViewTableRecord viewOld;
    GetCurrentView(viewOld);

    AcPlPlotEngine* pEngine = NULL;
    if (Acad::eOk == AcPlPlotFactory::createPublishEngine(pEngine))
    {
        std::set<CString> setPlotFile;

        // Here is the progress dialog for the current plot process...
        AcPlPlotProgressDialog* pPlotProgDlg = acplCreatePlotProgressDialog(acedGetAcadFrame()->m_hWnd, false, 1);
        pPlotProgDlg->setPlotMsgString(AcPlPlotProgressDialog::PlotMSGIndex::kDialogTitle, L"Plot API Progress");
        pPlotProgDlg->setPlotMsgString(AcPlPlotProgressDialog::PlotMSGIndex::kCancelJobBtnMsg, L"Cancel Job");
        pPlotProgDlg->setPlotMsgString(AcPlPlotProgressDialog::PlotMSGIndex::kCancelSheetBtnMsg, L"Cancel Sheet");
        pPlotProgDlg->setPlotMsgString(AcPlPlotProgressDialog::PlotMSGIndex::kSheetSetProgressCaption, L"Job Progress");
        pPlotProgDlg->setPlotMsgString(AcPlPlotProgressDialog::PlotMSGIndex::kSheetProgressCaption, L"Sheet Progress");
        pPlotProgDlg->setPlotProgressRange(0, 100);
        pPlotProgDlg->onBeginPlot();
        pPlotProgDlg->setIsVisible(true);

        int nDwgCount = plotSettingInfo.m_vecBlockInfo4Plot.size();
        for (int num = 0; num < plotSettingInfo.m_vecBlockInfo4Plot.size(); num++)
        {
            if (pPlotProgDlg->isPlotCancelled() || pPlotProgDlg->isSheetCancelled())
            {
                break;
            }

            CString csFilePathWithoutExt = plotSettingInfo.m_csSaveLocation + _T("\\") + CString(plotSettingInfo.m_vecBlockInfo4Plot[num].m_sPlotFileName.data());
            CString csDstPdf = GetNextFilePath(csFilePathWithoutExt, _T(".pdf"), 0, plotSettingInfo.m_bOverride);

            CString csStatus;
            csStatus.Format(_T("正在打印%d/%d..."), num + 1, nDwgCount);
            pPlotProgDlg->setPlotMsgString(AcPlPlotProgressDialog::PlotMSGIndex::kSheetName, csDstPdf);
            pPlotProgDlg->setPlotMsgString(AcPlPlotProgressDialog::PlotMSGIndex::kStatus, csStatus);

            AcPlPlotInfo plotInfo;
            SetPlotInfo(plotSettingInfo.m_vecBlockInfo4Plot[num], plotSettingInfo.m_idLayout, plotInfo);
            DoPlot(pPlotProgDlg, pEngine, plotInfo, true, csDstPdf);
            setPlotFile.insert(csDstPdf);

            //int nProgressPercent = (num + 1) * 100 / nDwgCount;
            pPlotProgDlg->setPlotProgressPos(100);
        }

        // Destroy the engine 
        pEngine->destroy();
        pEngine = NULL;

        // and the progress dialog.
        pPlotProgDlg->destroy();
        pPlotProgDlg = NULL;
    }
    else
    {
        // Ensure the engine is not already busy...
        AfxMessageBox(L"Plot Engine is Busy...");
    }

    // 恢复视图
    if (bNeedRestoreView)
    {
        // 将视图对象设置为当前视图
        acedSetCurrentView(&viewOld, NULL);
    }
}
