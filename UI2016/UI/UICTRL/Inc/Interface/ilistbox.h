#ifndef _UI_ILISTBOX_H_
#define _UI_ILISTBOX_H_
#include "..\..\..\UISDK\Inc\Interface\ilistctrlbase.h"
#include "..\..\..\UISDK\Inc\Interface\ilistitembase.h"

namespace UI
{
struct IListBoxItem;
class ListBox;
struct __declspec(uuid("FDDBC931-D9D3-4E0E-A6D1-F885B5CC3545"))
UICTRL_API IListBox : public IListCtrlBase
{
    bool  SetSel(int nIndex);
    int  GetSel();
    void  SetBindObject(IObject* pCombobox);
	IListBoxItem*  AddString(LPCTSTR szText);

	UI_DECLARE_INTERFACE_ACROSSMODULE(ListBox);
};

class ListBoxItemShareData;
struct IListBoxItemShareData : public IListItemShareData
{
    UI_DECLARE_INTERFACE_ACROSSMODULE(ListBoxItemShareData);
};


class ListBoxItem;
struct UICTRL_API IListBoxItem : public IListItemBase
{
	enum { FLAG = 155091039 };
    UI_DECLARE_INTERFACE_ACROSSMODULE(ListBoxItem);
};


}

#endif // _UI_ILISTBOX_H_