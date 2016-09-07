#include "stdafx.h"
#include "firstpagedlg.h"
#include "Dialog\Framework\MainFrame.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\ihyperlink.h"


CFirstPageDlg::CFirstPageDlg()
{
    m_pMainFrame = NULL;

    for (int i = 0; i <MAX_RECENT_COUNT; i++)
    {
        m_pHyperLinks[i] = NULL;
        m_pEraseBtns[i] = NULL;
    }
    m_pBtnNew = NULL;
    m_pBtnOpen = NULL;
}
void  CFirstPageDlg::SetMainFrame(CMainFrame* p)
{
    m_pMainFrame = p;
}

BOOL  CFirstPageDlg::PreCreateWindow(CREATESTRUCT* pcs)
{
    pcs->style = WS_CHILD;
    pcs->dwExStyle = 0;
    return TRUE;
}
void  CFirstPageDlg::OnInitialize()
{
    for (int i = 0; i <MAX_RECENT_COUNT; i++)
    {
        TCHAR szHyperlinkId[32] = _T("");
        TCHAR szEraseBtnId[32] = _T("");
        _stprintf(szHyperlinkId, _T("recent_hl_%d"), i+1);
        _stprintf(szEraseBtnId,  _T("recent_btn_%d"), i+1);

        m_pHyperLinks[i] = (IHyperLink*)FindChildObject(szHyperlinkId);
        m_pEraseBtns[i] = (IButton*)FindChildObject(szEraseBtnId);
    }
    m_pBtnNew = (IButton*)FindChildObject(_T("btnnew"));
    m_pBtnOpen = (IButton*)FindChildObject(_T("btnopen"));

    int nCount = g_pGlobalData->m_pRecentProj->GetItemCount();
    nCount = min(nCount, 5);  // 只显示前5个
    for (int i = 0; i < nCount; i++)
    {       
        RecentProjItem* pItem = g_pGlobalData->m_pRecentProj->GetItemByIndex(i);
        if (NULL == pItem)
            continue;

        if (!m_pHyperLinks[i] || !m_pEraseBtns[i])
            break;

        m_pHyperLinks[i]->SetText(pItem->szFilePath);
        m_pHyperLinks[i]->SetToolTipText(pItem->szFilePath);
        m_pHyperLinks[i]->SetVisible(true, false, true);
     //   m_pEraseBtns[i]->SetVisible(true, false, false);

        m_pHyperLinks[i]->SetUserData((LPVOID)pItem);
        m_pEraseBtns[i]->SetUserData((LPVOID)pItem);
    }
}

LRESULT  CFirstPageDlg::OnBtnClick(WPARAM w, LPARAM l)
{
    IMessage* pFrom = GetCurMsg()->pMsgFrom;
    for (int i = 0; i < MAX_RECENT_COUNT; i++)
    {
        if (pFrom == static_cast<IMessage*>(m_pHyperLinks[i]))
        {
            OnClickRecentItem((RecentProjItem*)m_pHyperLinks[i]->GetUserData());
            break;
        }
        if (pFrom == static_cast<IButton*>(m_pEraseBtns[i]))
        {
            OnEraseRecentItem((RecentProjItem*)m_pEraseBtns[i]->GetUserData());
            break;
        }
    }
    return 0;
}


void CFirstPageDlg::OnBtnNewProject()
{
    if (NULL == m_pMainFrame )
        return;

    m_pMainFrame->PostMessage(WM_COMMAND, ID_NEW_PROJECT);
}

void CFirstPageDlg::OnBtnOpenProject()
{
    m_pMainFrame->PostMessage(WM_COMMAND, ID_OPEN_PROJECT);
}

void  CFirstPageDlg::OnClickRecentItem(RecentProjItem* pItem)
{
    m_pMainFrame->PostMessage( WM_COMMAND, ID_OPEN_PROJECT, (LPARAM)pItem);
}
void  CFirstPageDlg::OnEraseRecentItem(RecentProjItem* pItem)
{

}

#if 0
void CFirstPageDlg::OnBtnRecentProject(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    HMENU hMenu = CreatePopupMenu();

    int nCount = g_pGlobalData->m_pRecentProj->GetItemCount();
    for (int i = 0; i < nCount; i++)
    {
        RecentProjItem* pItem = g_pGlobalData->m_pRecentProj->GetItemByIndex(i);
        if (NULL == pItem)
            continue;

        // 		TCHAR szLastVisitTime[64] = _T("");
        // 		_stprintf(szLastVisitTime, _T("%d"), pItem->nLastVisitTime);
        // 		m_listCtrlRecentProj.InsertItem(i, szLastVisitTime);
        // 		m_listCtrlRecentProj.SetItemText(i, 1, pItem->szFileName);
        // 		m_listCtrlRecentProj.SetItemText(i, 2, pItem->szFilePath);
        TCHAR szText[MAX_PATH] = _T("");
        _tcscpy(szText, pItem->szFilePath);
        Util::_WtlAbbreviateName(szText, 32, TRUE);
        ::AppendMenu(hMenu, MF_STRING, i+100, szText);
    }

    CWindow wnd = GetDlgItem(IDC_BTN_RECENT_PROJ);
    RECT rc;
    wnd.GetWindowRect(&rc);

    int nRet = ::TrackPopupMenu(hMenu, TPM_RETURNCMD, rc.left, rc.bottom, 0, m_hWnd, NULL);
    if (nRet >= 100)
    {
        m_pMainFrame->PostMessage( WM_COMMAND, ID_OPEN_PROJECT, 
            (LPARAM)g_pGlobalData->m_pRecentProj->GetItemByIndex(nRet-100) );
    }

    ::DestroyMenu(hMenu);
}

LRESULT CFirstPageDlg::OnDBClickRecentProj(LPNMHDR pnmh)
{
    LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) pnmh;

    m_pMainFrame->PostMessage( WM_COMMAND, ID_OPEN_PROJECT, 
        (LPARAM)g_pGlobalData->m_pRecentProj->GetItemByIndex(lpnmitem->iItem) );

    return 0;
}

LRESULT CFirstPageDlg::OnSelItemChanged(LPNMHDR pnmh)
{
    NMLISTVIEW* pItem = (NMLISTVIEW*)pnmh;
    if (pItem->uNewState & LVIS_SELECTED && 
        !(pItem->uOldState & LVIS_SELECTED))
    {
        m_pMainFrame->PostMessage( WM_COMMAND, ID_OPEN_PROJECT, 
            (LPARAM)g_pGlobalData->m_pRecentProj->GetItemByIndex(pItem->iItem) );
    }
    return 0;
}
#endif