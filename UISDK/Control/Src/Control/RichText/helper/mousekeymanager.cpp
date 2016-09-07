#include "stdafx.h"
#include "mousekeymanager.h"
#include "..\model\doc.h"
#include "..\model\page.h"
#include "..\model\line.h"
#include "..\model\run.h"

using namespace UI;
using namespace RichTextObjectModel;

RichTextMKMgr::RichTextMKMgr(Doc* pDoc)
{
	m_pDoc = pDoc;
	m_pUIApp = NULL;
	Reset();
}
RichTextMKMgr::~RichTextMKMgr()
{
    
}

void  RichTextMKMgr::Reset()
{
	m_pHoverPage = NULL;
	m_pHoverLine = NULL;
	m_pHoverRun = NULL;
	m_pHoverElement = NULL;
	m_pHoverUnit = NULL;
	m_pPressUnit = NULL;
}

void  RichTextMKMgr::SetUIApplication(IUIApplication* p)
{
	m_pUIApp = p;
}

bool  RichTextMKMgr::IsPageHover(RichTextObjectModel::Page*  pPage)
{
    if (pPage)
        return m_pHoverPage == pPage ? true:false;

    return false;
}
bool  RichTextMKMgr::IsLineHover(RichTextObjectModel::Line*  pLine)
{
    if (pLine)
        return m_pHoverLine == pLine ? true:false;

    return false;
}
bool  RichTextMKMgr::IsRunHover(RichTextObjectModel::Run*  pRun)
{
    if (pRun)
        return m_pHoverRun == pRun ? true:false;

    return false;
}
bool  RichTextMKMgr::IsElementHover(RichTextObjectModel::Element*  pElement)
{
    if (pElement)
        return m_pHoverElement == pElement ? true:false;

    return false;
}

void  RichTextMKMgr::OnMouseMove(UINT nFlags, POINT ptDoc)
{
	HitTest(ptDoc, &m_pHoverPage, &m_pHoverLine, &m_pHoverRun);

    RichTextObjectModel::Unit* pUnit = NULL;
    if (m_pHoverRun)
    {
        m_pHoverElement = m_pHoverRun->GetElement();
        if (m_pHoverElement)
            pUnit = m_pHoverElement->GetUnit();
    }

	RichTextObjectModel::Unit* pOldHover = m_pHoverUnit;
    SetHoverUnit(pUnit);

	// 更新鼠标样式
	if (!m_pPressUnit && pOldHover != m_pHoverUnit)
	{
		if (!m_pHoverUnit || !m_pHoverUnit->OnSetCursor())
		{
			static HCURSOR sArrowHandle = LoadCursor(NULL, IDC_ARROW);
			SetCursor(sArrowHandle);
		}
	}

    m_pDoc->Refresh();
}

void  RichTextMKMgr::OnMouseLeave()
{
	m_pHoverPage = NULL;
	m_pHoverLine = NULL;
	m_pHoverRun = NULL;
	m_pHoverElement = NULL;
	SetHoverUnit(NULL);
	m_pDoc->Refresh();
}

BOOL  RichTextMKMgr::OnSetCursor()
{
	if (m_pPressUnit)
		return m_pPressUnit->OnSetCursor();
	else if (m_pHoverUnit)
		return m_pHoverUnit->OnSetCursor();

	return FALSE;
}

void  RichTextMKMgr::OnLButtonDown(UINT nFlags, POINT point)
{
	if (m_pHoverUnit)
	{
		SetPressUnit(m_pHoverUnit);
	}

	m_pDoc->Refresh();
}

void  RichTextMKMgr::OnLButtonUp(UINT nFlags, POINT point)
{
	if (m_pPressUnit && m_pHoverUnit == m_pPressUnit)
		m_pPressUnit->OnClick();

	SetPressUnit(NULL);
}

void  RichTextMKMgr::SetHoverUnit(RichTextObjectModel::Unit* pUnit)
{
    if (pUnit == m_pHoverUnit)
        return;

	if (m_pUIApp)
		m_pUIApp->HideToolTip();
	if (pUnit)
	{
		TOOLTIPITEM item = {0};
		item.szToolTipContent = pUnit->GetToolTipText();
		if (item.szToolTipContent && item.szToolTipContent[0])
			m_pUIApp->ShowToolTip(&item);
	}

    RichTextObjectModel::Unit*  pOldUnit = m_pHoverUnit;
    m_pHoverUnit = pUnit;
    if (pOldUnit)
        pOldUnit->SetHover(false);
    if (m_pHoverUnit)
        m_pHoverUnit->SetHover(true);
}

void  RichTextMKMgr::SetPressUnit(RichTextObjectModel::Unit* pUnit)
{
	if (pUnit == m_pPressUnit)
		return;

	if (m_pUIApp)
		m_pUIApp->HideToolTip();

	RichTextObjectModel::Unit*  pOldUnit = m_pPressUnit;
	m_pPressUnit = pUnit;
	if (pOldUnit)
		pOldUnit->SetPress(false);
	if (m_pPressUnit)
		m_pPressUnit->SetPress(true);
}

#if 0
if (m_pHoverPage)
{
	if (!m_pHoverPage->IsVisible())
	{
		m_pHoverPage = NULL;
	}
	else
	{
		RECT rcPage;
		POINT ptDoc = {0};
		m_pHoverPage->GetPageContentRegion(&ptDoc, &rcPage);
		if (!PtInRect(rcPage, ptDoc))
			m_pHoverPage = NULL;
	}
}

if (!m_pHoverPage)
{
	Page* p = m_pDoc->GetFirstPage();
	while (p)
	{

	}
}
#endif
void  RichTextMKMgr::HitTest(POINT ptDoc, Page** ppHoverPage, Line** ppHoverLine, Run** ppHoverRun)
{
	if (ppHoverPage)
		*ppHoverPage = NULL;
	if (ppHoverLine)
		*ppHoverLine = NULL;
	if (ppHoverRun)
		*ppHoverRun = NULL;

	// Page测试
	POINT ptInPage = {0};
	Page* pHoverPage = m_pDoc->PageHitTest(ptDoc, &ptInPage);
	if (!pHoverPage)
		return;

	if (ppHoverPage)
		*ppHoverPage = pHoverPage;

	// Line测试
	POINT ptInLine = {0};
	Line* pHoverLine = pHoverPage->LineHitTest(ptInPage, &ptInLine);
	if (!pHoverLine)
		return;

	if (ppHoverLine)
		*ppHoverLine = pHoverLine;

	// Run测试
	POINT ptInRun = {0};
	Run* pHoverRun = pHoverLine->RunHitTest(ptInLine, &ptInRun);
	if (!pHoverRun)
		return;

	if (ppHoverRun)
		*ppHoverRun = pHoverRun;
}