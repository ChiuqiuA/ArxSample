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
    // ��ӡ�ļ���Ϣ
    std::vector<BlockInfo> m_vecBlockInfo4Plot;

    // �������
    CString m_csSaveLocation;
    BOOL	m_bOverride = FALSE;

    // ��ӡͼ��
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
        // ����BACKGROUNDPLOT
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
        // �ָ�ԭBACKGROUNDPLOT
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

    // ��ӡ������
    es = pPSV->setPlotCfgName(pLayoutNew.get(), CString(blockInfo.m_sDevice_Name.data()));

    // ���ô�ӡֽ��
    es = pPSV->setCanonicalMediaName(pLayoutNew.get(), CString(blockInfo.m_sPaper_Setting.data()));
    //es = pPSV->setClosestMediaName(pLayoutNew.get(), paperWidth, paperHeight, units, matchPrintableArea);    // ��������ʵĴ�ӡֽ��

    // ���ô�ӡ��ʽ��
    es = pPSV->setCurrentStyleSheet(pLayoutNew.get(), CString(blockInfo.m_sPlot_Setting.data()));

    // ��ӡ����
    es = pPSV->setUseStandardScale(pLayoutNew.get(), true);
    es = pPSV->setStdScaleType(pLayoutNew.get(), AcDbPlotSettings::kScaleToFit);   // ʹ��ö��ֵ���ñ�׼���ţ�����ʹ��setStdScale���ݸ���ֵ���ñ�׼����
    //es = pPSV->setCustomPrintScale(pLayoutNew.get(), numerator, denominator);  // �����Զ����ӡ����
    //es = pPSV->setPlotPaperUnits(pLayoutNew.get(), units);  // ����ֽ�ŵ�λ��Ӣ�ߡ����ס�����

    // ��ӡƫ��
    es = pPSV->setPlotCentered(pLayoutNew.get(), true);
    //es = pPSV->setPlotOrigin(pLayoutNew, xCoordinate, yCoordinate);  // ���ô�ӡƫ�ƣ�ԭ�������ڿɴ�ӡ����

    // ����ҳ����ת
    es = pPSV->setPlotRotation(pLayoutNew.get(), PlotRotation::k0degrees);

    // ��ӡ��Χ
    if (blockInfo.m_Extents.minPoint() == AcGePoint3d::kOrigin && blockInfo.m_Extents.maxPoint() == AcGePoint3d::kOrigin)
    {
        es = pPSV->setPlotType(pLayoutNew.get(), AcDbPlotSettings::kExtents);
    }
    else
    {
        AcGePoint3d ptMinDcs = WcsToDcsPoint(blockInfo.m_Extents.minPoint() - AcGeVector3d(1, 1, 0));
        AcGePoint3d ptMaxDcs = WcsToDcsPoint(blockInfo.m_Extents.maxPoint() + AcGeVector3d(1, 1, 0));
        es = pPSV->setPlotWindowArea(pLayoutNew.get(), ptMinDcs.x, ptMinDcs.y, ptMaxDcs.x, ptMaxDcs.y);// ���ô�ӡ��������

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
    // ����ϵͳ����BACKGROUNDPLOT
    SystemVar backgroudplot(_T("BACKGROUNDPLOT"), 0);

    // ��ȡԭ��ͼ
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
            csStatus.Format(_T("���ڴ�ӡ%d/%d..."), num + 1, nDwgCount);
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

    // �ָ���ͼ
    if (bNeedRestoreView)
    {
        // ����ͼ��������Ϊ��ǰ��ͼ
        acedSetCurrentView(&viewOld, NULL);
    }
}
