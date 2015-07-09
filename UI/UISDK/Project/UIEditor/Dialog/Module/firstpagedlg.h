#pragma once
#include "Other\resource.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
    interface IHyperLink;
    interface IButton;
}

#define MAX_RECENT_COUNT  5  // 只显示5个最近的工程

//
//	首页，创建/打开工程
//
class CFirstPageDlg : public  IWindow
{
public:
    CFirstPageDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CFirstPageDlg)
        UIMSG_BN_CLICKED2(m_pBtnNew, OnBtnNewProject)
        UIMSG_BN_CLICKED2(m_pBtnOpen, OnBtnOpenProject)
        UIMSG_WM_NOTIFY(UI_BN_CLICKED, OnBtnClick)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    void  SetMainFrame(CMainFrame* p);
    void  OnInitialize();
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    LRESULT  OnBtnClick(WPARAM w, LPARAM l);
    void  OnBtnNewProject();
    void  OnBtnOpenProject();

    void  OnClickRecentItem(RecentProjItem* pItem);
    void  OnEraseRecentItem(RecentProjItem* pItem);

    void  OnBtnRecentProject(UINT uNotifyCode, int nID, CWindow wndCtl);
    LRESULT  OnDBClickRecentProj(LPNMHDR pnmh);
    LRESULT  OnSelItemChanged(LPNMHDR pnmh);

private:
    CMainFrame*   m_pMainFrame;

    IHyperLink*   m_pHyperLinks[MAX_RECENT_COUNT];
    IButton*      m_pEraseBtns[MAX_RECENT_COUNT];
    IButton*      m_pBtnNew;
    IButton*      m_pBtnOpen;
};