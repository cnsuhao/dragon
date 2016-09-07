#include "stdafx.h"
#include "projdependsctrldllconfigdlg.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "Business\Project\DependsCtrlDll\dependsctrldll.h"
#include "Dialog\Framework\statusbar.h"

CProjectDependsCtrlDllConfigDlg::CProjectDependsCtrlDllConfigDlg()
{
    m_pBtnAdd = NULL;
    m_pBtnDel = NULL;
    m_pBtnOk = NULL;
    m_pListBox = NULL;
}
CProjectDependsCtrlDllConfigDlg::~CProjectDependsCtrlDllConfigDlg()
{

}

void CProjectDependsCtrlDllConfigDlg::OnInitialize()
{
    m_pListBox = (IListBox*)FindChildObject(_T("listbox"));
    m_pBtnAdd = (IButton*)FindChildObject(_T("btn_add"));
    m_pBtnDel = (IButton*)FindChildObject(_T("btn_del"));
    m_pBtnOk = (IButton*)FindChildObject(_T("btn_ok"));

    if (m_pBtnDel)
    {
        m_pBtnDel->SetEnable(false, false);
    }

    CDependsCtrlDllMgr* pMgr = GetProjDependsCtrlDllMgr();
    if (!pMgr)
        return;
    for (UINT i = 0; i < pMgr->m_vStrDlls.size(); i++)
    {
        m_pListBox->AddString(pMgr->m_vStrDlls[i]->strAbsolutePath.c_str(), 0);
    }
    m_pListBox->LayoutItem(NULL, false);
}   

void  CProjectDependsCtrlDllConfigDlg::OnBtnAdd()
{
    CFileDialog dlg(TRUE, _T(".dll"), NULL, 0, _T("*.dll\0*.dll\0\0"), GetHWND());
    if (IDCANCEL == dlg.DoModal())
        return ;

    HMODULE hModule = ::LoadLibrary(dlg.m_szFileName);
    FARPROC pfun = ::GetProcAddress(hModule, "RegisterUIObject");
    FreeLibrary(hModule);
    if (!pfun)
    {
        TCHAR szText[320] = _T("");
        _stprintf(szText, _T("在%s中未找到RegisterUIObject导出函数"), dlg.m_szFileName);
        g_pGlobalData->m_pStatusBar->SetStatusText2(szText);
        return;
    }

    if (!GetProjDependsCtrlDllMgr()->AddCtrlDll(dlg.m_szFileName))
        return;

    if (m_pListBox)
    {
        m_pListBox->AddString(dlg.m_szFileName, UI::LF_REDRAW|UI::LF_LAYOUT);
    }
}
void  CProjectDependsCtrlDllConfigDlg::OnBtnDel()
{
    if (!m_pListBox)
        return;

    IListItemBase* pSel = m_pListBox->GetFirstSelectItem();
    if (!pSel)
        return;

    if (GetProjDependsCtrlDllMgr()->DelCtrlDll(pSel->GetText()))
    {
        m_pListBox->RemoveItem(pSel, UI::LF_REDRAW|UI::LF_LAYOUT);
    }
}

LRESULT  CProjectDependsCtrlDllConfigDlg::OnListBoxSelChanged(WPARAM w, LPARAM l)
{
    if (!m_pBtnDel)
        return 0;

    IListItemBase* pNew = (IListItemBase*)l;
    m_pBtnDel->SetEnable(pNew ? true:false);

    return 0;
}

void  CProjectDependsCtrlDllConfigDlg::OnBtnOk()
{
    EndDialog(IDOK);
}