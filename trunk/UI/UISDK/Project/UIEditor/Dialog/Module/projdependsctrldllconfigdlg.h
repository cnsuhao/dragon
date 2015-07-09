#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"

namespace UI
{ 
    interface IButton; 
}

class CProjectDependsCtrlDllConfigDlg : public IWindow
{
public:
    CProjectDependsCtrlDllConfigDlg();
    ~CProjectDependsCtrlDllConfigDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CProjectDependsCtrlDllConfigDlg)
        UIMSG_BN_CLICKED2(m_pBtnAdd, OnBtnAdd)
        UIMSG_BN_CLICKED2(m_pBtnDel, OnBtnDel)
        UIMSG_BN_CLICKED2(m_pBtnOk, OnBtnOk)

        UIMSG_WM_NOTIFY_CODE_FROM(UI_LCN_SELCHANGED, m_pListBox, OnListBoxSelChanged)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)


protected:
    void  OnInitialize();
    void  OnBtnAdd();
    void  OnBtnDel();
    void  OnBtnOk();
    LRESULT  OnListBoxSelChanged(WPARAM w, LPARAM l);

private:
    IButton*   m_pBtnAdd;
    IButton*   m_pBtnDel;
    IButton*   m_pBtnOk;
    IListBox*   m_pListBox;
};