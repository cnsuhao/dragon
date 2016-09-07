#ifndef ILISTBOX_H_FFF69C0C_35D2_4ca2_985B_10E38FAB46C4
#define ILISTBOX_H_FFF69C0C_35D2_4ca2_985B_10E38FAB46C4
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{
interface IListBoxItem;
class ListBox;
interface UICTRLAPI IListBox : public IListCtrlBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListBox, ListBox);

    bool  SetSel(int nIndex);
    void  SetBindObject(IObject* pCombobox);
	IListBoxItem*  AddString(LPCTSTR szText, int nAddItemFlags);
};

class ListBoxItemShareData;
interface IListBoxItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IListBoxItemShareData, ListBoxItemShareData);
};


class ListBoxItem;
interface UICTRLAPI IListBoxItem : public IListItemBase
{
	enum { FLAG = 155091039 };
    UI_DECLARE_Ixxx_INTERFACE(IListBoxItem, ListBoxItem);
};


}

#endif // ILISTBOX_H_FFF69C0C_35D2_4ca2_985B_10E38FAB46C4