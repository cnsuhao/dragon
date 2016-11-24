#include "stdafx.h"
#include "listbox.h"
#include "listboxitem.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ListBox, ListCtrlBase)
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ListBoxItem, ListItemBase)
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(ListBoxItemShareData, ListItemShareData)

void  IListBox::SetBindObject(IObject* pCombobox)
{
    __pImpl->SetBindObject(pCombobox);
}

IListBoxItem*  IListBox::AddString(LPCTSTR szText)
{
    return __pImpl->AddString(szText);
}

bool  IListBox::SetSel(int nIndex)
{
    return __pImpl->SetSel(nIndex);
}
int  IListBox::GetSel()
{
    return __pImpl->GetSel();
}

}