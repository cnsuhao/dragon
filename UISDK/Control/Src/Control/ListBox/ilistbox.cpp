#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "UISDK\Control\Src\Control\ListBox\listbox.h"
#include "UISDK\Control\Src\Control\ListBox\listboxitem.h"

namespace UI
{
    
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IListBox, ListBox, IListCtrlBase)
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IListBoxItem, ListBoxItem, IListItemBase)
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IListBoxItemShareData, ListBoxItemShareData, IListItemTypeShareData)

void  IListBox::SetBindObject(IObject* pCombobox)
{
    m_pListBoxImpl->SetBindObject(pCombobox);
}

IListBoxItem*  IListBox::AddString(LPCTSTR szText, int nAddItemFlags)
{
    return m_pListBoxImpl->AddString(szText, nAddItemFlags);
}

bool  IListBox::SetSel(int nIndex)
{
    return m_pListBoxImpl->SetSel(nIndex);
}


}