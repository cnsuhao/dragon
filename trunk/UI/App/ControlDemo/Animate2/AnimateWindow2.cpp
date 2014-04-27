#include "StdAfx.h"
#include "AnimateWindow2.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"


CAnimateWindow2::CAnimateWindow2()
{
    m_pBtnShowNextTransition = NULL;
    m_isAnimating = false;
    m_nAnimateCursor = 0;
    m_nCurrentPage = 0;
    m_pCurPage = NULL;
    m_pNextPage = NULL;

    m_sizeWindow.cx = 0; 
    m_sizeWindow.cy = 0;

    for (int i = 0; i < PAGE_COUNT; i++)
        m_pPages[i] = NULL;
}

CAnimateWindow2::~CAnimateWindow2(void)
{
    UI::IUIApplication*  pUIApp = GetUIApplication();
    UI::IAnimateManager* pAniMgr = pUIApp->GetAnimateMgr();
    pAniMgr->ClearStoryboardOfNotify(this);
}

void CAnimateWindow2::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);

	String  strText(_T("Animate -- "));
	switch(GetIWindowRender()->GetGraphicsRenderType())
	{
	case UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		strText.append(XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDI);
		break;
	case UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
		strText.append(XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDIPLUS);
		break;
	case UI::GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
		strText.append(XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D2D);
		break;
	}
	SetWindowText(GetHWND(), strText.c_str());

    m_pBtnShowNextTransition = (UI::IButton*)FindChildObject(_T("btnShowNext"));

    m_pPages[0] = (UI::IPanel*)FindChildObject(_T("page-1"));
    for (int i = 1; i < PAGE_COUNT; i++)
        m_pPages[i] = (UI::IPanel*)m_pPages[i-1]->GetNextObject();

    m_pMenu1 = (UI::IListBox*)FindChildObject(_T("menu_1"));
    if (m_pMenu1)
    {
        m_pMenu1->AddString(_T("MOVE TO LEFT / FROM RIGHT"), LISTITEM_OPFLAG_NOALL)->SetData(0);
        m_pMenu1->AddString(_T("MOVE TO RIGHT / FROM LEFT"), LISTITEM_OPFLAG_NOALL)->SetData(1);
        m_pMenu1->AddString(_T("MOVE TO TOP / FROM BOTTOM"), LISTITEM_OPFLAG_NOALL)->SetData(2);
        m_pMenu1->AddString(_T("MOVE TO BOTTOM / FROM TOP"), LISTITEM_OPFLAG_NOALL)->SetData(3);
        m_pMenu1->UpdateItemRect();
    }
}

void  CAnimateWindow2::OnSize(UINT nType, int cx, int cy)
{
    if (nType == SIZE_MINIMIZED)
        return;

    m_sizeWindow.cx = cx;
    m_sizeWindow.cy = cy;
}

void  CAnimateWindow2::OnAnimateOver()
{
    if (m_pCurPage)
        m_pCurPage->SetVisible(false, false, false);
    
    m_pNextPage = NULL;
    m_pCurPage = NULL;
    m_isAnimating = false;
}
void  CAnimateWindow2::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
    for (int i = 0; i < nCount; i++)   
    {
        UI::IStoryboard*  pStoryboard = ppArray[0];
        long  lTransitionType = pStoryboard->GetWParam();

        switch (lTransitionType)
        {
        case 0:
        case 1:
            {
                int  nCurX = 0, nNextX = 0;
                pStoryboard->GetTimeline(0)->GetCurrentValue(&nCurX);
                pStoryboard->GetTimeline(1)->GetCurrentValue(&nNextX);

                m_pCurPage->SetObjectPos(nCurX, 0, m_sizeWindow.cx, m_sizeWindow.cy, SWP_NOREDRAW);
                m_pNextPage->SetObjectPos(nNextX, 0, m_sizeWindow.cx, m_sizeWindow.cy, SWP_NOREDRAW);
            }
            break;

        case 2:
        case 3:
            {
                int  nCurY = 0, nNextY = 0;
                pStoryboard->GetTimeline(0)->GetCurrentValue(&nCurY);
                pStoryboard->GetTimeline(1)->GetCurrentValue(&nNextY);

                m_pCurPage->SetObjectPos(0, nCurY, m_sizeWindow.cx, m_sizeWindow.cy, SWP_NOREDRAW);
                m_pNextPage->SetObjectPos(0, nNextY, m_sizeWindow.cx, m_sizeWindow.cy, SWP_NOREDRAW);
            }
            break;
        }
    }
    //::InvalidateRect(GetHWND(), NULL, TRUE);
    this->UpdateObject();
}

void  CAnimateWindow2::OnClickShowNextTransition()
{
    if (m_isAnimating)
        return;

    if (m_nAnimateCursor >= ANIMATE_COUNT)
        m_nAnimateCursor = 0;
    
    nextPage(m_nAnimateCursor);
    m_nAnimateCursor++;
}

void  CAnimateWindow2::nextPage(long lType)
{
    if (m_isAnimating)
        return;
    m_isAnimating = true;

    m_pCurPage = m_pPages[m_nCurrentPage];
    m_nCurrentPage ++;
    if (m_nCurrentPage >= PAGE_COUNT)
        m_nCurrentPage = 0;
    m_pNextPage = m_pPages[m_nCurrentPage];
    m_pNextPage->SetVisible(true, false, false);

    UI::IUIApplication*  pUIApp = GetUIApplication();
    UI::IAnimateManager* pAniMgr = pUIApp->GetAnimateMgr();
    UI::IStoryboard*  pStoryboard = pAniMgr->CreateStoryboard(this, 0, (WPARAM)lType);

    if (lType >= 0 && lType <= 3)
    {
        UI::IIntTimeline*  pCurTimeline = pStoryboard->CreateIntTimeline(TIMELINE_ID_CUR);
        UI::IIntTimeline*  pNextTimeline = pStoryboard->CreateIntTimeline(TIMELINE_ID_NEXT);

        switch (lType)
        {
        case 0:
            pCurTimeline->SetEaseParam(0, -m_sizeWindow.cx, 600, UI::ease);
            pNextTimeline->SetEaseParam(m_sizeWindow.cx, 0, 600, UI::ease);
            break;
        case 1:
            pCurTimeline->SetEaseParam(0, m_sizeWindow.cx, 600, UI::ease);
            pNextTimeline->SetEaseParam(-m_sizeWindow.cx, 0, 600, UI::ease);
            break;
        case 2:
            pCurTimeline->SetEaseParam(0, -m_sizeWindow.cy, 600, UI::ease);
            pNextTimeline->SetEaseParam(m_sizeWindow.cy, 0, 600, UI::ease);
            break;
        case 3:
            pCurTimeline->SetEaseParam(0, m_sizeWindow.cy, 600, UI::ease);
            pNextTimeline->SetEaseParam(-m_sizeWindow.cy, 0, 600, UI::ease);
            break;
        }
    }
  
    pStoryboard->Begin();
}

LRESULT  CAnimateWindow2::OnListBoxClick(WPARAM w, LPARAM l)
{
    if (GetCurMsg()->pMsgFrom == m_pMenu1)
    {
        UI::IListItemBase*  pClickItem = (UI::IListItemBase*)l;
        nextPage(pClickItem->GetData());
    }

    return 0;
}