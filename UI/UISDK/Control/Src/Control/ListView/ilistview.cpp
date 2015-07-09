#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ilistview.h"
#include "UISDK\Control\Src\Control\listview\listview.h"
#include "UISDK\Control\Src\Control\ListView\listviewitembase.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IListView, ListView, IListCtrlBase);
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IListViewItemBase, ListViewItemBase, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IListViewItemBaseShareData, ListViewItemBaseShareData, IListItemTypeShareData);

IHeaderListCtrl*  IListView::GetHeaderCtrl()                       
{
    return m_pListViewImpl->GetHeaderCtrl();
}

IListViewItemBase*  IListView::AddTextItem(LPCTSTR szText, int nAddFlag)     
{
    return m_pListViewImpl->AddTextItem(szText, nAddFlag); 
}

IListViewItemBase*  IListView::InsertTextItem(int nIndex, LPCTSTR szText, int nInsertFlag) 
{ 
    return m_pListViewImpl->InsertTextItem(nIndex, szText, nInsertFlag);
}
bool  IListView::SetItemText(int nItem, int nSubItem, LPCTSTR szText, bool bUpdate) 
{
    return m_pListViewImpl->SetItemText(nItem, nSubItem, szText, bUpdate);
}

bool  IListViewItemBase::SetSubItemText(int nSub, LPCTSTR szText)
{
	return m_pListViewItemBaseImpl->SetSubItemText(nSub, szText);
}
LPCTSTR  IListViewItemBase::GetSubItemText(int nSub)
{
	return m_pListViewItemBaseImpl->GetSubItemText(nSub);
}
}