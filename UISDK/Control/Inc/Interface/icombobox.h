#ifndef ICOMBOBOX_H_2548D733_F520_415e_9D3E_1DE433307792
#define ICOMBOBOX_H_2548D733_F520_415e_9D3E_1DE433307792
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

   
#define UI_CBN_SELCHANGED  132331846

// 
//  点击了下拉按钮或者ComboBox，通知子类去弹出下拉列表
//      message: UI_WM_NOTIFY
//      wParam:
//      lParam:
//      lRet:   0:未处理  1:已处理
//
#define UI_CBN_DROPDOWN  132331847

//  要求关闭下拉列表
//      message: UI_WM_NOTIFY
//      code: UI_CBN_CLOSEUP
//      
#define UI_CBN_CLOSEUP  132331848

    
// void OnCbnSelChanged(UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem)
#define UIMSG_CBN_SELCHANGED(ptr, func)               \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        pMsgFrom == (UI::IMessage*)ptr &&             \
        code == UI_CBN_SELCHANGED )                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UI::IListItemBase*)wParam, (UI::IListItemBase*)lParam); \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

#define COMBOBOX_BUTTON_MSG_HOOK_MSG_ID  1
#define COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID 2
#define COMBOBOX_EDIT_MSG_HOOK_MSG_ID    3
#define COMBOBOX_LISTBOX_MSG_NOTIFY_MSG_ID 4

#define COMBOBOX_BUTTON_ID _T("__combo_button")
#define COMBOBOX_EDIT_ID   _T("__combo_edit")
#define COMBOBOX_LIST_ID   _T("__combo_list")

interface IListBoxItem;
interface IListBox;
interface IEdit;

class ComboBoxBase;
interface UICTRLAPI IComboBoxBase : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IComboBoxBase, ComboBoxBase);

    LPCTSTR  GetText();
	void  SetText(LPCTSTR szText, bool bUpdate);
	IObject*  GetContentObject(LPCTSTR szText);
    void  CloseUp();
    IEdit*  GetEdit();
};


class ComboBox;
interface UICTRLAPI IComboBox : public IComboBoxBase
{
    UI_DECLARE_Ixxx_INTERFACE(IComboBox, ComboBox)

    bool  AddString(LPCTSTR, int nAddItemFlags=0);
    IListBoxItem*  AddStringEx(LPCTSTR, int nAddItemFlags=0);
    bool  SetCurSel(int nIndex);
    IListBox*  GetListBox();
	void  ResetContent();
	IEdit*  GetEdit();

    void  SetReadOnly(bool b);
    bool  IsReadOnly();
};

}

#endif // ICOMBOBOX_H_2548D733_F520_415e_9D3E_1DE433307792