#include "stdafx.h"
#include "listviewitembase.h"
#include "UISDK\Control\Inc\Interface\ilistview.h"

namespace UI
{

ListViewItemBaseShareData::ListViewItemBaseShareData()
{
    m_pIListViewItemBaseShareData = NULL;
    m_pHeaderCtrl = NULL;
}
ListViewItemBaseShareData::~ListViewItemBaseShareData()
{

}

ListViewItemBase::ListViewItemBase()
{
    m_pIListViewItemBase = NULL;
    m_pShareData = NULL;

}
ListViewItemBase::~ListViewItemBase()
{
    DestroySubItems();
}

void  ListViewItemBase::DestroySubItems()
{
	vector<SubItemInfo>::iterator iter = m_vecSubItem.begin();
	for (; iter != m_vecSubItem.end(); ++iter)
	{
		iter->Destroy();
	}
    m_vecSubItem.clear();
}

void  ListViewItemBase::OnInitialize()
{
    DO_PARENT_PROCESS(IListViewItemBase, IListItemBase);

    IListCtrlBase* pCtrl = m_pIListViewItemBase->GetIListCtrlBase();
    m_pIListViewItemBase->SetItemType(LISTITEM_TYPE_LISTVIEWITEMBASE);

    IListViewItemBaseShareData*  pData = static_cast<IListViewItemBaseShareData*>
        (pCtrl->GetItemTypeShareData(LISTITEM_TYPE_LISTVIEWITEMBASE));

    if (NULL == pData)
    {
        IListViewItemBaseShareData::CreateInstance(pCtrl->GetSkinRes(), &pData);
        m_pShareData = pData->GetImpl();
        pCtrl->SetItemTypeShareData(LISTITEM_TYPE_LISTVIEWITEMBASE, pData);

        m_pShareData->m_pHeaderCtrl = (IHeaderListCtrl*)UISendMessage(pCtrl, UI_LVM_GETHEADERCTRL);
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }

    if (m_pShareData->m_pHeaderCtrl)
    {
        int nCount = m_pShareData->m_pHeaderCtrl->GetItemCount();
        if (nCount > 1)
			m_vecSubItem.resize(nCount - 1);

		vector<SubItemInfo>::iterator iter = m_vecSubItem.begin();
		for (; iter != m_vecSubItem.end(); ++iter)
			iter->Init();
    }
}

void  ListViewItemBase::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIListViewItemBase->GetIListCtrlBase();

    CRect  rcParent;
    m_pIListViewItemBase->GetParentRect(&rcParent);

    // ±³¾°
    UINT nRenderState = 0;
    IRenderBase* pRender = pListCtrl->GetForeRender();
    if (pRender)
    {
        nRenderState = m_pIListViewItemBase->GetItemRenderState();
        pRender->DrawState(pRenderTarget, &rcParent, nRenderState);
    }

    if (m_pIListViewItemBase->IsFocus() && !m_pIListViewItemBase->IsSelected())
    {
        IRenderBase* pFocusRender = pListCtrl->GetFocusRender();
        if (pFocusRender)
        {
            bool bCtrlFocus = pListCtrl->IsFocus();
            pFocusRender->DrawState(pRenderTarget, &rcParent, bCtrlFocus ?
                    LISTCTRLITEM_FOCUS_RENDER_STATE_NORMAL:LISTCTRLITEM_FOCUS_RENDER_STATE_NORMAL);
        }
    }

    // ÎÄ±¾
	if (!m_pShareData)
		return;
    if (!m_pShareData->m_pHeaderCtrl)
        return;

    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (!pTextRender)
        pTextRender = pListCtrl->CreateDefaultTextRender();
    if (!pTextRender)
        return;

    CRect rcSubItem = rcParent;
    rcSubItem.right = rcSubItem.left;

    CRect  rcItem;
    m_pIListViewItemBase->GetParentRect(&rcItem);

    IListItemBase*  pColItem = m_pShareData->m_pHeaderCtrl->GetFirstItem();
    ColumnInfo info = {0};
    info.nMask = CIM_INDENT_LEFT | CIM_INDENT_RIGHT;

    while (pColItem)
    {
        CRect rcSubItem;
        pColItem->GetParentRect(&rcSubItem);
        UISendMessage(pColItem, UI_HDM_GETITEM_COLUMNINFO, (WPARAM)&info);

        rcSubItem.top = rcItem.top;
        rcSubItem.bottom = rcItem.bottom;

        if (0 == rcSubItem.Width())
        {
            pColItem = pColItem->GetNextItem();
            continue;
        }

        rcSubItem.left += info.nIndentLeft;
        rcSubItem.right -= info.nIndentRight;

        DrawSubItemData data = {pColItem->GetId(), pColItem->GetLineIndex(), pRenderTarget, pTextRender, pColItem, &rcSubItem, nRenderState};
        OnDrawSubItem(&data);

        pColItem = pColItem->GetNextItem(); 
    }
}

void  ListViewItemBase::OnDrawSubItem(DrawSubItemData* pSubItem)
{
    ColumnInfo info = {0};
    info.nMask = CIM_FORMAT;
    UISendMessage(pSubItem->pItem, UI_HDM_GETITEM_COLUMNINFO, (WPARAM)&info);
    if (0 == pSubItem->nColumnIndex)
    {
        pSubItem->pTextRender->DrawState(
            pSubItem->pRenderTarget, 
            pSubItem->prc, 
            pSubItem->nRenderState, 
            m_pIListViewItemBase->GetText(),
            info.nFormat);

        return;
   }

   UINT nIndex = pSubItem->nColumnIndex - 1;
   if (nIndex >= m_vecSubItem.size())
       return;

   LPCTSTR szText = m_vecSubItem[nIndex].szText;
   if (szText)
   {
	   pSubItem->pTextRender->DrawState(
		   pSubItem->pRenderTarget, 
		   pSubItem->prc, 
		   pSubItem->nRenderState, 
		   szText,
		   info.nFormat);
   }
}

bool  ListViewItemBase::SetSubItemText(int nSub, LPCTSTR szText)
{
	UINT nIndex = (UINT)nSub;
	if (0 == nIndex)
	{
		m_pIListViewItemBase->SetText(szText);
		return true;
	}

	if (nIndex > m_vecSubItem.size())
		return false;

	m_vecSubItem[nIndex-1].SetText(szText);
	return true;
}
LRESULT  ListViewItemBase::OnSetSubItemText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetSubItemText((int)wParam, (LPCTSTR)lParam);
	return 0;
}

LPCTSTR  ListViewItemBase::GetSubItemText(int nSub)
{
	UINT nIndex = (UINT)nSub;
	if (0 == nIndex)
		return m_pIListViewItemBase->GetText();

	if (nIndex > m_vecSubItem.size())
		return NULL;

	return m_vecSubItem[nIndex-1].szText;
}
LRESULT  ListViewItemBase::OnGetSubItemText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)GetSubItemText((int)wParam);
}

LRESULT  ListViewItemBase::OnAddColumn(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT nPos = (UINT)wParam;
    if (nPos > m_vecSubItem.size())
        return 0;

    _MyIter iter = m_vecSubItem.begin();
    for (UINT i = 0; i < nPos; i++)
    {
        iter++;
    }
	SubItemInfo info = {0};
    m_vecSubItem.insert(iter, 1, info);
    return 0;
}

LRESULT  ListViewItemBase::OnRemoveColumn(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT nPos = (UINT)wParam;
    if (nPos >= m_vecSubItem.size())
        return 0;

    if (m_vecSubItem.size() <= 1)
    {
        DestroySubItems();
    }
    else
    {
        _MyIter iter = m_vecSubItem.begin();
		iter += (nPos-1);

		if (iter != m_vecSubItem.end())
		{
			iter->Destroy();
	        m_vecSubItem.erase(iter);
		}
    }
    return 0;
}

LRESULT  ListViewItemBase::OnClearColumn(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DestroySubItems();
    return 0;
}

}