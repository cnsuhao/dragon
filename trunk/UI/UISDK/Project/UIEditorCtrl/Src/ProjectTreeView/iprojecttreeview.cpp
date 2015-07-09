#include "stdafx.h"
#include "UISDK\Project\UIEditorCtrl\Inc\iprojecttreeview.h"
#include "UISDK\Project\UIEditorCtrl\Src\ProjectTreeView\projecttreeview.h"
#include "UISDK\Project\UIEditorCtrl\Src\ProjectTreeView\skinitem\skintreeviewitem.h"
#include "UISDK\Project\UIEditorCtrl\Src\ProjectTreeView\controlitem\controltreeviewitem.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IProjectTreeView, ProjectTreeView, ITreeView)
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(ISkinTreeViewItem, SkinTreeViewItem, INormalTreeItem)
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IControlTreeViewItem, ControlTreeViewItem, INormalTreeItem)

ISkinTreeViewItem*  IProjectTreeView::InsertSkinItem(const TCHAR* szText, IListItemBase* pParent, IListItemBase* pInsertAfter , int nInsertFlags)
{
    return m_pProjectTreeViewImpl->InsertSkinItem(szText, pParent, pInsertAfter, nInsertFlags);    
}
IControlTreeViewItem*  IProjectTreeView::InsertControlItem(const TCHAR* szText, IListItemBase* pParent, IListItemBase* pInsertAfter , int nInsertFlags)
{
    return m_pProjectTreeViewImpl->InsertControlItem(szText, pParent, pInsertAfter, nInsertFlags);    
}

void  ISkinTreeViewItem::SetTextRender(ITextRenderBase**  ppTextRender)
{
    m_pSkinTreeViewItemImpl->SetTextRender(ppTextRender);
}
}