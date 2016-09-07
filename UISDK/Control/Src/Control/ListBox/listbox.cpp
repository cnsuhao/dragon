#include "stdafx.h"
#include "listbox.h"
#include "listboxitem.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"

namespace UI
{

bool ListBoxCompareProc(IListItemBase* p1, IListItemBase* p2);

ListBox::ListBox()
{
    m_pIListBox = NULL;
    m_pBindObject = NULL;
	
}
ListBox::~ListBox()
{
}

void  ListBox::OnInitialize()
{
	DO_PARENT_PROCESS(IListBox, IListCtrlBase);
	m_pIListBox->SetSortCompareProc(ListBoxCompareProc);
	m_vscrollbarCreator.Initialize(m_pIListBox, m_pIListBox->GetIScrollBarMgr());
}
void  ListBox::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
	DO_PARENT_PROCESS_MAPID(IListBox, IListCtrlBase, UIALT_CALLLESS);
	m_vscrollbarCreator.CreateByEditor(pData, m_pIListBox);
}

void ListBox::SetBindObject(IObject* pCombobox)
{
    m_pBindObject = pCombobox;
}

bool  ListBoxCompareProc(IListItemBase* pItem1, IListItemBase* pItem2)
{
    if (pItem1->GetSort() == pItem2->GetSort())
    {
        return (_tcscoll(pItem1->GetText(), pItem2->GetText()) < 0);
    }
    return pItem1->GetSort() < pItem2->GetSort();
}


bool ListBox::SetSel(int nIndex)
{
	IListItemBase* pOldSelItem = m_pIListBox->GetFirstSelectItem();

	// 清除当前选项
	if (-1 == nIndex && NULL != pOldSelItem)
	{
		m_pIListBox->SelectItem(NULL, false);
	}
	else
	{
		IListItemBase* pItem = m_pIListBox->GetItemByPos(nIndex);
		if (false == pItem)
			return false;

		m_pIListBox->SelectItem(pItem, false);
	}

	m_pIListBox->Refresh();
	return true;
}

bool  ListBox::IsPopup()
{
	ListCtrlStyle s = {0};
	s.popuplistbox = 1;
    return m_pIListBox->TestListCtrlStyle(&s);
}
void ListBox::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    DO_PARENT_PROCESS(IListBox, IListCtrlBase);

#if 0
    IUIApplication* pUIApplication = m_pIListBox->GetUIApplication();
    IScrollBarManager*  pScrollBarMgr = m_pIListBox->GetIScrollBarMgr();
    if (pScrollBarMgr)
    {
        if (!pScrollBarMgr->GetHScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(HSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemHScrollBar*  p = NULL;
            ISystemHScrollBar::CreateInstance(pUIApplication, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pIListBox->AddNcChild(p);
            p->SetId(XML_HSCROLLBAR_PRIFIX);
            
            p->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            p->SetVisible(false, false, false);
        }

        if (!pScrollBarMgr->GetVScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(VSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemVScrollBar*  p = NULL;
            ISystemVScrollBar::CreateInstance(pUIApplication, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pIListBox->AddNcChild(p);
            p->SetId(XML_VSCROLLBAR_PRIFIX);

            p->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            p->SetVisible(false, false, false);
            pScrollBarMgr->SetVScrollWheel(20);
        }
    }
#endif
}

IListBoxItem*  ListBox::AddString(LPCTSTR szText, int nAddItemFlags)
{
    IListBoxItem* pItem = NULL;
    IListBoxItem::CreateInstance(m_pIListBox->GetSkinRes(), &pItem);

    if (NULL == pItem)
        return NULL;

    if (szText)
        pItem->SetText(szText);

    m_pIListBox->AddItem(pItem, nAddItemFlags);
    return pItem;
}
bool ListBox::RemoveItem(IListBoxItem* pItem, int nFlag)
{
    if (NULL == pItem)
        return false;

    m_pIListBox->RemoveItem(pItem, nFlag);
    return true;
}
}