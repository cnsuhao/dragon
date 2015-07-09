#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\itreeview.h"
#include "UISDK\Control\Src\Control\TreeView\treeitem.h"
#include "UISDK\Control\Src\Control\TreeView\treeview.h"
#include "UISDK\Control\Src\Control\TreeView\normaltreeitem.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(ITreeItem, TreeItem, IListItemBase)
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(ITreeView, TreeView, IListCtrlBase)
UI_IMPLEMENT_Ixxx_INTERFACE(INormalTreeItem, NormalTreeItem, TreeItem)
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(INormalTreeItemShareData, NormalTreeItemShareData, IListItemTypeShareData);

IRenderBase*  ITreeView::GetExpandIconRender()  
{
    return m_pTreeViewImpl->GetExpandIconRender(); 
}
ITreeItem*  ITreeView::InsertNormalItem(UITVITEM* pItem, IListItemBase* pParent, IListItemBase* pInsertAfter, int nInsertFlags)
{
    return m_pTreeViewImpl->InsertNormalItem(pItem, pParent, pInsertAfter, nInsertFlags); 
}
ITreeItem*  ITreeView::InsertNormalItem(LPCTSTR szText, IListItemBase* pParent, IListItemBase* pInsertAfter, int nInsertFlags)
{ 
    return m_pTreeViewImpl->InsertNormalItem(szText, pParent, pInsertAfter, nInsertFlags); 
}

int  INormalTreeItemShareData::GetImageWidth()
{
    return m_pNormalTreeItemShareDataImpl->GetImageWidth();
}
int  INormalTreeItemShareData::GetExpandIconWidth()
{
    return m_pNormalTreeItemShareDataImpl->GetExpandIconWidth();
}
IRenderBase*  INormalTreeItemShareData::GetExpandIconRender()
{
	return m_pNormalTreeItemShareDataImpl->pExpandIconRender;
}