#include "stdafx.h"
#include "listctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemRootPanel\listitemrootpanel.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemBase\listitembase.h"
#include <algorithm>
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\MouseKeyboard\popuplistctrlmkmgr.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\MouseKeyboard\multisellistctrlmkmgr.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\MouseKeyboard\menumkmgr.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer.h"
using namespace UI;


//
//  �ڲ��ؼ���ˢ������.
//  ע. �ڲ��ؼ��ĸ�������Ȼ��LISTCTRL����LISTCTRLû�н��ڲ��ؼ���Ϊ�Լ�����Ԫ�أ��������
//      ��������ʱ���ܿ�ܶࡣ�Ӷ���Ļ�����ITEM��ֱ�ӵ���RootPanel::DrawObject��ʵ�֣�ͬ
//      ʱ��RootPanel����Ϊ������͸����Ȼ���޸�RootPanel.OnErasebkgndΪRedrawItemʵ���ڲ�
//      ���ֵı���ˢ��
//
//  1. ����ˢ��
//     . ListCtrl::OnPaint ->
//     . ListCtrl::OnDrawItem, ListItem::OnDrawItem, ListItem::DrawItemInnerControl ->
//	   . RootPanel::DrawObject
//
//  2. ����ITEM REDRAW
//     . ListCtrl::redraw_item
//     . ListCtrl::OnDrawItem, ListItem::OnDrawItem, ListItem::DrawItemInnerControl ->
//	   . RootPanel::DrawObject
//
//  3. �����ڲ��ؼ�ˢ��
//     . Button.UpdateObject
//     . WindowBase._InnerRedraw(���������begindraw��ʹ��ListCtrl::RedrawItem�е���begindrawpart����isdrawingΪtrue)
//     . RootPanel.OnEraseBkgnd(rootpanel�ı�����͸�������������������)
//     . ListCtrl::redraw_item
//     . ListCtrl::OnDrawItem, ListItem::OnDrawItem (���ﲻ�ٵ���ListItem::OnDrawItemInnerControl�ˣ���Ϊ��һ�����ڵ�2���ѻ���)
//     . Button.ParentPanel.OnEraseBkgnd
//     . Button.DrawObject                                                                                                                                                                                                                                                                                                          
//     
//

//////////////////////////////////////////////////////////////////////////

ListItemTypeShareData::ListItemTypeShareData()
{
    m_pIListItemTypeShareData = NULL;
    m_pListCtrlBase = NULL;
}
ListItemTypeShareData::~ListItemTypeShareData()
{

}
void  ListItemTypeShareData::SetListCtrlBase(ListCtrlBase* p)
{
    m_pListCtrlBase = p;
}
IListCtrlBase*  ListItemTypeShareData::GetListCtrlBase()
{
    if (m_pListCtrlBase)
        return m_pListCtrlBase->GetIListCtrlBase();
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
    
ListCtrlBase::ListCtrlBase()
{
    m_pIListCtrlBase = NULL;

	m_pFirstItem         = NULL;
	m_pLastItem          = NULL;
	m_pFirstDrawItem     = NULL;
	m_pFirstSelectedItem = NULL;
	m_pLastDrawItem      = NULL;
	m_pCompareProc       = NULL;
    m_pFocusRender       = NULL;

	m_nItemCount         = 0;
	m_nItemHeight        = 20;
	m_nVertSpacing       = 0;
	m_nHorzSpacing       = 0;
    m_nChildNodeIndent   = 20;
    SetRectEmpty(&m_rItemContentPadding);
	memset(&m_listctrlStyle, 0, sizeof(m_listctrlStyle));

	m_sizeMax.cx = NDEF;
	m_sizeMax.cy = NDEF;
	m_sizeMin.cx = NDEF;
	m_sizeMin.cy = NDEF;

	m_bNeedCalcFirstLastDrawItem = false;
    m_bRedrawInvalidItems = false;
    m_bPaintingCtrlRef = 0;
	m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_NONE;
	for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
		m_pInvalidateItems[i] = NULL;

    m_pMKMgr = NULL;
	m_pEditingItem = NULL;

    SetClipClient(true);
}

ListCtrlBase::~ListCtrlBase()
{
    SAFE_DELETE(m_pMKMgr);
    SAFE_RELEASE(m_pFocusRender);
}

HRESULT  ListCtrlBase::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IListCtrlBase, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_MgrLayout.SetListCtrlBase(this);
    m_MgrFloatItem.SetListCtrlBase(this);

	if (NULL == m_MgrLayout.GetLayout())
    {
		SetLayout(CreateListCtrlLayout(LISTCTRL_ITEM_LAYOUT_TYPE_1, m_pIListCtrlBase));
    }
    if (NULL == m_pMKMgr)
    {
        m_pMKMgr = new SingleSelListCtrlMKMgr;
        m_pMKMgr->SetListCtrlBase(p->GetImpl(), this);
    }

    m_mgrScrollBar.SetBindObject(static_cast<IObject*>(m_pIListCtrlBase));

	ObjStyle s = {0};
	s.vscroll = 1;
	s.hscroll = 1;
	this->ModifyObjectStyle(&s, 0);

	return S_OK;
}

void  ListCtrlBase::FinalRelease()
{
    ListCtrlStyle s = {0};
    s.destroying = 1;
    ModifyListCtrlStyle(&s, 0);

    m_mgrScrollBar.SetHScrollBar(NULL); // �������������ˣ�_RemoveAllItem�л���ù���������
    m_mgrScrollBar.SetVScrollBar(NULL);  

    this->_RemoveAllItem();   // ��Ϊ_RemoveAllItem������麯��������������û�з�������������ִ��

    // ���ٹ�������
    map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.begin();
    for (; iter != m_mapItemTypeShareData.end(); iter++)
    {
        iter->second->delete_this();
    }
    m_mapItemTypeShareData.clear();
    m_mapItem.clear();  

    DO_PARENT_PROCESS(IListCtrlBase, IControl);
}

void  ListCtrlBase::SetIListCtrlBase(IListCtrlBase* p)
{
    m_pIListCtrlBase = p; 
}
IListCtrlBase*  ListCtrlBase::GetIListCtrlBase()
{
    return m_pIListCtrlBase; 
}

void  ListCtrlBase::OnSerialize(SERIALIZEDATA* pData)
{
	DO_PARENT_PROCESS(IListCtrlBase, IControl);

	{
		AttributeSerializer  s(pData, TEXT("ListCtrl"));
		s.AddLong(XML_LISTCTRL_ITEM_HEIGHT, m_nItemHeight)
            ->SetDefault(20);
		s.AddLong(XML_LISTCTRL_MIN_WIDTH, m_sizeMin.cx)
            ->SetDefault(NDEF);
		s.AddLong(XML_LISTCTRL_MIN_HEIGHT, m_sizeMin.cy)
            ->SetDefault(NDEF);
		s.AddLong(XML_LISTCTRL_MAX_WIDTH, m_sizeMax.cx)
            ->SetDefault(NDEF);
		s.AddLong(XML_LISTCTRL_MAX_HEIGHT, m_sizeMax.cy)
            ->SetDefault(NDEF);
		s.AddRect(XML_LISTCTRL_ITEM_PADDING, m_rItemContentPadding);
	}

	m_mgrScrollBar.Serialize(pData);
	m_mgrScrollBar.SetVScrollLine(m_nItemHeight);
	m_mgrScrollBar.SetHScrollLine(3);

	// text render
// 	if (NULL == m_pIListCtrlBase->GetTextRender())
// 	{
//         ITextRenderBase* pTextRender = NULL;
//         pMapAttrib->GetAttr_TextRenderBase(NULL, XML_TEXTRENDER_TYPE, true, pUIApp, m_pIListCtrlBase, &pTextRender);
//         if (pTextRender)
//         {
//             m_pIListCtrlBase->SetTextRender(pTextRender);
//             SAFE_RELEASE(pTextRender);
//         }
//     }
//     if (NULL == m_pIListCtrlBase->GetTextRender())
//     {
//         ITextRenderBase* pTextRender = NULL;
//         pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_SIMPLE, m_pIListCtrlBase, &pTextRender);
//         if (pTextRender)
//         {
//             SERIALIZEDATA data = {0};
//             data.pUIApplication = GetIUIApplication();
//             data.pMapAttrib = pMapAttrib;
//             data.szPrefix = NULL;
//             data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
//             pTextRender->Serialize(&data);
// 
//             m_pIListCtrlBase->SetTextRender(pTextRender);
//             pTextRender->Release();
//         }
// 	}

    if (pData->IsLoad())
    {
        map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.begin();
        for (; iter != m_mapItemTypeShareData.end(); iter++)
        {
		    SERIALIZEDATA data = {0};
            data.pUIApplication = GetIUIApplication();
		    data.pMapAttrib = pData->pMapAttrib;
		    data.nFlags = SERIALIZEFLAG_LOAD;
		    if (data.IsReload())
			    data.nFlags |= SERIALIZEFLAG_RELOAD;
		    UISendMessage(iter->second, UI_WM_SERIALIZE, (WPARAM)&data);
            //UISendMessage(iter->second, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)bReload);
        }
    }
}

void  ListCtrlBase::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
	pData->rcInitPos.right = pData->rcInitPos.left + 100;
	pData->rcInitPos.bottom = pData->rcInitPos.top + 100;
}

IScrollBarManager*  ListCtrlBase::GetIScrollBarMgr()
{
    return m_mgrScrollBar.GetIScrollBarMgr();
}

void ListCtrlBase::SetLayout(IListCtrlLayout* pLayout)
{
    if (pLayout)
        pLayout->SetIListCtrlBase(m_pIListCtrlBase);
    
    m_MgrLayout.SetLayout(pLayout);
}

// ��������������ΪListCtrlItemLayout1
void  ListCtrlBase::SetLayoutFixedHeight()
{
    this->SetLayout(new ListCtrlItemSimpleLayout());
}
// �ɱ�߶�
void  ListCtrlBase::SetLayoutVariableHeight()
{
    this->SetLayout(new ListCtrlItemVariableHeightLayout());
}
IListCtrlLayout*  ListCtrlBase::GetLayout()
{
    return m_MgrLayout.GetLayout();
}

void ListCtrlBase::RemoveItem(int nIndex, int nRemoveFlag)
{
	ListItemBase* pItem = this->GetItemByPos(nIndex, true);
	if (NULL == pItem)
		return ;

	this->RemoveItem(pItem, nRemoveFlag);
}

void  ListCtrlBase::DelayRemoveItem(ListItemBase* pItem, int nRemoveFlag)
{
    IUIApplication* pUIApplication = m_pIListCtrlBase->GetUIApplication();
    if (NULL == pUIApplication)
        return;

    ListCtrlStyle s = {0};
    s.destroying = 1;
    if (TestListCtrlStyle(&s))
        return;

    // TODO: ��α�֤����Ӧ֮ǰpItem�������٣�
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_LCN_INNER_DELAY_REMOVE_ITEM;
    msg.wParam = (WPARAM)pItem;
    msg.lParam = (LPARAM)nRemoveFlag;
    msg.pMsgFrom = m_pIListCtrlBase;
    msg.pMsgTo = m_pIListCtrlBase;
    UIPostMessage(pUIApplication, &msg);

    // ����ˢ�¸��
    RemoveInvavlidateItem(pItem);
}

LRESULT  ListCtrlBase::OnDelayRemoveItem(WPARAM w, LPARAM l)
{
    RemoveItem((ListItemBase*)w, (int)l);
    return 0;
}

void ListCtrlBase::RemoveItem(ListItemBase* pItem, int nRemoveFlag)
{
	if (NULL == pItem)
		return;

	ListItemBase* pNextItem = pItem->GetNextItem();

	bool bRet = _RemoveItem(pItem);
	if (false == bRet)
		return;

	this->UpdateItemIndex(pNextItem);
	
	if (nRemoveFlag & LF_LAYOUT)
	{
		this->LayoutItem(pNextItem, false);
	}
	if (nRemoveFlag & LF_REDRAW)
	{
		m_pIListCtrlBase->UpdateObject();
	}
}

void  ListCtrlBase::RemoveAllChildItems(ListItemBase* pParent, int nRemoveFlag)
{
    if (NULL == pParent)
        return;

    ListItemBase* pParentNext = pParent->GetNextItem();
    bool bRet = _RemoveAllChildItems(pParent);
    if (false == bRet)
        return;

    if (nRemoveFlag & LF_UPDATEITEMINDEX)
    {
        this->UpdateItemIndex(pParentNext);
    }

    if (nRemoveFlag & LF_LAYOUT)
    {
        this->LayoutItem(pParentNext, false);
    }
    if (nRemoveFlag & LF_REDRAW)
    {
        m_pIListCtrlBase->UpdateObject();
    }
}

bool  ListCtrlBase::_RemoveAllChildItems(ListItemBase* pParent)
{
    if (NULL == pParent)
        return false;

    ListItemBase*  pItem = pParent->GetChildItem();
    if (NULL == pItem)
        return false;

    ListItemBase*  pFocusItem = GetFocusItem();
    if (pParent->IsMyChildItem(pFocusItem, true))
    {
        SetFocusItem(pParent);
    }

    bool bSelChanged = false;
    while (pItem)
    {
        m_nItemCount--;

        if (m_pMKMgr)
            m_pMKMgr->OnRemoveItem(pItem, &bSelChanged);
        m_MgrFloatItem.OnRemoveItem(pItem);

        // ����Ч�б����Ƴ�
        RemoveInvavlidateItem(pItem);

        {
            long lId = pItem->GetId();

            UIMSG  msg;
            msg.pMsgTo = m_pIListCtrlBase;
            msg.message = UI_WM_NOTIFY;
            msg.nCode  = UI_LCN_ITEMREMOVE;
            msg.wParam = (WPARAM)&lId;
            msg.lParam = (LPARAM)pItem->GetIListItemBase();
            UISendMessage(&msg);
        }

        ListItemBase* pNext = pItem->GetNextItem();
        pItem->GetIListItemBase()->delete_this();;
        pItem = pNext;
    }

    pParent->SetChildItem(NULL);
    pParent->SetLastChildItem(NULL);
    this->SetCalcFirstLastDrawItemFlag();

    // ������״̬���ݸ��º���֪ͨ��Ҫ��Ȼ�ⲿ�����֪ͨ�п��ܴ����µ�ˢ�²���
    if (bSelChanged)
        FireSelectItemChanged(NULL);

    return true;
}


//
// item�����ˡ�
// ���ڽ����༶�������ؼ�����ɾ������һ�������ʱ����Ҫ�������ӽ�㶼��һ����������
//
void  ListCtrlBase::ItemDestructed(ListItemBase* pItem)
{
    if (!pItem)
        return;

    ListCtrlStyle s = {0};
    s.destroying = 1;
    if (TestListCtrlStyle(&s))
        return;

    long lId = pItem->GetId();
    if (lId != 0)
    {
        _mapItemIter iter = m_mapItem.find(lId);
        if (iter != m_mapItem.end())
            m_mapItem.erase(iter);
    }
}


// δˢ�£�δ����Item Rect, scroll bar
bool ListCtrlBase::_RemoveItem(ListItemBase* pItem)
{
	if (NULL == pItem)
		return false;
	
    long lIdSave = pItem->GetId();

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_WM_NOTIFY;
        msg.wParam = (WPARAM)pItem->GetIListItemBase();
        msg.nCode = UI_LCN_PRE_ITEMREMOVE;
        UISendMessage(&msg);
    }

    bool  bSelChanged = false;
    if (m_pMKMgr)
        m_pMKMgr->OnRemoveItem(pItem, &bSelChanged);
    m_MgrFloatItem.OnRemoveItem(pItem);

    RemoveItemFromTree(pItem);

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_WM_NOTIFY;
        msg.nCode  = UI_LCN_ITEMREMOVE;
        msg.wParam = (WPARAM)lIdSave;
        msg.lParam = (LPARAM)pItem->GetIListItemBase();
        UISendMessage(&msg);
    }

    pItem->GetIListItemBase()->delete_this();;

    // ������״̬���ݸ��º���֪ͨ��Ҫ��Ȼ�ⲿ�����֪ͨ�п��ܴ����µ�ˢ�²���
    if (bSelChanged)
        FireSelectItemChanged(NULL);

	return true;
}
void ListCtrlBase::RemoveAllItem(int nRemoveFlag)
{
    bool bHaveSelection = m_pFirstSelectedItem?true:false;
	if (false == _RemoveAllItem())
		return;

    if (bHaveSelection)
        FireSelectItemChanged(NULL);

	if (nRemoveFlag & LF_LAYOUT)
		this->LayoutItem(m_pFirstDrawItem, false);

	if (nRemoveFlag & LF_REDRAW)
		m_pIListCtrlBase->UpdateObject();
}
bool ListCtrlBase::_RemoveAllItem()
{
	if (NULL == m_pFirstItem)
		return false;

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_LCN_PRE_ALLITEMREMOVE;
        UISendMessage(&msg);
    }

	ListItemBase* p = m_pFirstItem;
	while (p)
	{
		ListItemBase* pNext = p->GetNextItem();  // Save
		p->GetIListItemBase()->delete_this();
		p = pNext;
	}

	m_pFirstItem = NULL;
	m_pLastItem = NULL;
	m_pFirstDrawItem = NULL;
	m_pFirstSelectedItem = NULL;
	m_nItemCount = 0;
    m_mapItem.clear();
    if (m_pMKMgr)
        m_pMKMgr->OnRemoveAll();
    m_MgrFloatItem.OnRemoveAllItem();

	m_mgrScrollBar.SetScrollRange(0,0);

    SetInvalidateAllItems();

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_LCN_ALLITEMREMOVE;
        UISendMessage(&msg);
    }
	return true;
}
void ListCtrlBase::SetSortCompareProc(ListItemCompareProc p)
{
	m_pCompareProc = p;
}

void  ListCtrlBase::Sort()
{
    if (NULL == m_pCompareProc)
        return;

    SortChildren(NULL);
}

void  ListCtrlBase::SortChildren(ListItemBase* pParent)
{
    if (NULL == m_pCompareProc)
        return;

    if (!m_listctrlStyle.sort_ascend && !m_listctrlStyle.sort_descend)
        return;

    if (!pParent)
    {
        sort_by_first_item(m_pFirstItem, GetRootItemCount(), 
			m_listctrlStyle.sort_child);
    }
    else
    {
        sort_by_first_item(pParent->GetChildItem(), pParent->GetChildCount(),
			m_listctrlStyle.sort_child);
    }

    this->SetCalcFirstLastDrawItemFlag();
}

// nCountΪpFirstItem���ھ����� 
void  ListCtrlBase::sort_by_first_item(ListItemBase* pFirstItem, int nNeighbourCount, bool bSortChildren)
{
    if (!pFirstItem)
        return;

    if (nNeighbourCount <= 1)
    {
        // ֱ�����ӽڵ�
        if (pFirstItem->GetChildItem() && bSortChildren)
        {
            sort_by_first_item(pFirstItem->GetChildItem(), pFirstItem->GetChildCount(), bSortChildren);
        }
        return;
    }

    IListItemBase**  pArray = new IListItemBase*[nNeighbourCount];
    pArray[0] = pFirstItem->GetIListItemBase();

    for (int i = 1; i < nNeighbourCount; i++)
    {
        pArray[i] = pArray[i-1]->GetNextItem();
    }

    std::sort(pArray, (pArray+nNeighbourCount), m_pCompareProc);

    // �������������������б�
    if (m_listctrlStyle.sort_ascend)
    {
        ListItemBase*  pFirstItem = pArray[0]->GetImpl();
        ListItemBase*  pLastItem = pArray[nNeighbourCount-1]->GetImpl();

        ListItemBase*  pParentItem = pFirstItem->GetParentItem();
        if (pParentItem)
        {
            pParentItem->SetChildItem(pFirstItem);
            pParentItem->SetLastChildItem(pLastItem);            
        }
        else
        {
            m_pFirstItem = pFirstItem;
            m_pLastItem = pLastItem;
        }

        pFirstItem->SetPrevItem(NULL);
        pFirstItem->SetNextItem(pArray[1]->GetImpl());

        pLastItem->SetNextItem(NULL);
        pLastItem->SetPrevItem(pArray[nNeighbourCount-2]->GetImpl());

        for (int i = 1; i < nNeighbourCount-1; i++)
        {
            pArray[i]->SetNextItem(pArray[i+1]);
            pArray[i]->SetPrevItem(pArray[i-1]);
        }
    }
    else
    {
        ListItemBase*  pLastItem = pArray[0]->GetImpl();
        ListItemBase*  pFirstItem = pArray[nNeighbourCount-1]->GetImpl();

        ListItemBase*  pParentItem = pFirstItem->GetParentItem();
        if (pParentItem)
        {
            pParentItem->SetChildItem(pFirstItem);
            pParentItem->SetLastChildItem(pLastItem);            
        }
        else
        {
            m_pFirstItem = pFirstItem;
            m_pLastItem = pLastItem;
        }

        pFirstItem->SetPrevItem(NULL);
        pFirstItem->SetNextItem(pArray[nNeighbourCount-2]->GetImpl());

        pLastItem->SetNextItem(NULL);
        pLastItem->SetPrevItem(pArray[1]->GetImpl());

        for (int i = 1; i < nNeighbourCount-1; i++)
        {
            pArray[i]->SetPrevItem(pArray[i+1]);
            pArray[i]->SetNextItem(pArray[i-1]);
        }
    }

    SAFE_ARRAY_DELETE(pArray);

    if (bSortChildren)
    {
        ListItemBase*  pItem = pFirstItem;
        while (pItem)
        {
            if (pItem->GetChildItem())
                sort_by_first_item(pItem->GetChildItem(), pItem->GetChildCount(), bSortChildren);

            pItem = pItem->GetNextItem();
        }
    }
}

// ��������item��λ��
void  ListCtrlBase::SwapItemPos(ListItemBase*  p1, ListItemBase* p2)
{
    if (NULL == p1 || NULL == p2 || p1 == p2)
        return;

    if (p1->GetNextItem() == p2)
    {
        ListItemBase* p1Prev = p1->GetPrevItem();
        ListItemBase* p2Next = p2->GetNextItem();

        if (p1Prev)
            p1Prev->SetNextItem(p2);
        p2->SetPrevItem(p1Prev);

        if (p2Next)
            p2Next->SetPrevItem(p1);
        p1->SetNextItem(p2Next);

        p2->SetNextItem(p1);
        p1->SetPrevItem(p2);
    }
    else if (p1->GetPrevItem() == p2)
    {
        ListItemBase* p1Next = p1->GetNextItem();
        ListItemBase* p2Prev = p2->GetPrevItem();

        if (p2Prev)
            p2Prev->SetNextItem(p1);
        p1->SetPrevItem(p2Prev);

        if (p1Next)
            p1Next->SetPrevItem(p2);
        p2->SetNextItem(p1Next);

        p1->SetNextItem(p2);
        p2->SetPrevItem(p1);
    }
    else
    {
        ListItemBase* p1Prev = p1->GetPrevItem();
        ListItemBase* p1Next = p1->GetNextItem();

        ListItemBase* p2Prev = p2->GetPrevItem();
        ListItemBase* p2Next = p2->GetNextItem();

        if (p2Next)
            p2Next->SetPrevItem(p1);
        p1->SetNextItem(p2Next);

        if (p2Prev)
            p2Prev->SetNextItem(p1);
        p1->SetPrevItem(p2Prev);

        if (p1Next)
            p1Next->SetPrevItem(p2);
        p2->SetNextItem(p1Next);

        if (p1Prev)
            p1Prev->SetNextItem(p2);
        p2->SetPrevItem(p1Prev);       
    }

    // ������ʼ��
    if (NULL == p1->GetPrevItem())
        m_pFirstItem = p1;
    if (NULL == p2->GetPrevItem())
        m_pFirstItem = p2;

    if (NULL == p1->GetNextItem())
        m_pLastItem = p1;
    if (NULL == p2->GetNextItem())
        m_pLastItem = p2;

    // ����������
    int nIndex1 = p1->GetLineIndex();
    int nIndex2 = p2->GetLineIndex();
    p1->SetLineIndex(nIndex2);
    p2->SetLineIndex(nIndex1);

    // ���¿ɼ�
    this->SetCalcFirstLastDrawItemFlag();

    // ˢ��item rect
    this->LayoutAllItemsAndRedraw();
}

int  ListCtrlBase::GetChildNodeIndent()
{
    return m_nChildNodeIndent;
}
void  ListCtrlBase::SetChildNodeIndent(int n)
{
    m_nChildNodeIndent = n;
}

void ListCtrlBase::SetItemHeight(int nHeight, bool bUpdate)
{
	if (m_nItemHeight == nHeight)
		return;

	m_nItemHeight = nHeight;
//	this->MeasureAllItem();

    if (bUpdate)
    	this->LayoutItem(m_pFirstItem, true);
}
ListItemBase*  ListCtrlBase::GetItemByWindowPoint(POINT pt)
{
    if (!m_pMKMgr)
        return NULL;

    return m_pMKMgr->GetItemByPos(pt);
}
ListItemBase*  ListCtrlBase::GetItemUnderCursor()
{
    POINT pt = {0};
    GetCursorPos(&pt);
    MapWindowPoints(NULL, m_pIListCtrlBase->GetHWND(), &pt, 1);
    return GetItemByWindowPoint(pt);
}
ListItemBase* ListCtrlBase::GetItemByPos(UINT nIndex, bool bVisibleOnly)
{
	if (nIndex < 0)
		return NULL;

    if (bVisibleOnly)
    {
        ListItemBase* pItem = FindVisibleItemFrom(NULL);

        UINT i = 0;
        while (pItem)
        {
            if (i == nIndex)
                return pItem;

            i++;
            pItem = pItem->GetNextVisibleItem();
        }
    }
    else
    {
        if (nIndex >= m_nItemCount)
            return NULL;

	    ListItemBase* pItem = m_pFirstItem;

	    UINT i = 0;
	    while (pItem)
	    {
		    if (i == nIndex)
			    return pItem;

		    i++;
		    pItem = pItem->GetNextItem();
	    }
    }
    return NULL;
}


ListItemBase*  ListCtrlBase::GetItemById(long lId)
{
    if (0 == lId)
    {
        ListItemBase* pItem = m_pFirstItem;

        while (pItem)
        {
            if (pItem->GetId() == lId)
                return pItem;

            pItem = pItem->GetNextTreeItem();
        }
        return NULL;
    }
    else
    {
        _mapItemIter iter = m_mapItem.find(lId);
        if (iter == m_mapItem.end())
            return NULL;

        return iter->second->GetImpl();
    }
}
void  ListCtrlBase::ItemIdChanged(IListItemBase* pItem, long lOldId, long lNewId)
{
    if (!pItem)
        return;

    if (lOldId == lNewId)
        return;

    if (0 != lOldId)
    {
        _mapItemIter iter = m_mapItem.find(lOldId);
        if (iter != m_mapItem.end())
            m_mapItem.erase(iter);
    }
    if (0 != lNewId)
    {
        m_mapItem[lNewId] = pItem;
    }
}
ListItemBase* ListCtrlBase::FindItemByText(LPCTSTR  szText, ListItemBase* pStart)
{
    if (NULL == szText)
        return NULL;

    ListItemBase* p = pStart;
    if (NULL == pStart)
        p = m_pFirstItem;

    if (NULL == p)
        return NULL;

    while (p)
    {
        if (0 == _tcscmp(p->GetText(), szText))
        {
            return p;
        }
        p = p->GetNextTreeItem();
    }

    return NULL;
}

ListItemBase*  ListCtrlBase::EnumItemByProc(ListItemEnumProc pProc, ListItemBase* pEnumFrom, WPARAM w, LPARAM l)
{
    if (!pProc)
        return NULL;

    ListItemBase* p = pEnumFrom;
    if (NULL == pEnumFrom)
        p = m_pFirstItem;

    if (NULL == p)
        return NULL;

    while (p)
    {
        if (!pProc(p->GetIListItemBase(), w, l))
        {
            return p;
        }
        p = p->GetNextTreeItem();
    }

    return NULL;
}

// ����pParent�²��ң����pParentΪNULL,�����������
ListItemBase*  ListCtrlBase::FindChildItemByText(LPCTSTR  szText, ListItemBase* pParent, ListItemBase* pStart)
{
    if (NULL == szText)
        return NULL;

    if (!pStart)
    {
        if (pParent)
            pStart = pParent->GetChildItem();
        else
            pStart = m_pFirstItem;
    }

    if (pStart)
    {
        ListItemBase* p = pStart;
        while (p)
        {
            if (0 == _tcscmp(p->GetText(), szText))
            {
                return p;
            }
            p = p->GetNextItem();
        }
    }
    return NULL;
}

//
//	��ĩβ���һ�������������������InsertItem
//
bool ListCtrlBase::AddItem(ListItemBase* pItem, int nAddItemFlags)
{
	ListItemBase* pInsertAfter = m_pLastItem;

	bool bAscendSort = m_listctrlStyle.sort_ascend;
	bool bDescendSort = m_listctrlStyle.sort_descend;

	if ((bAscendSort||bDescendSort) && m_pCompareProc && (nAddItemFlags & LF_SORT))
	{
		// �������λ��(���ڲ��������ӵ����ݽṹ�����ܲ��ö��ֲ��ҵķ�ʽ...)
		ListItemBase* pEnumItem = m_pFirstItem;
		while (NULL != pEnumItem)
		{
			int nResult = m_pCompareProc(pEnumItem->GetIListItemBase(), 
				pItem->GetIListItemBase());
			if (bAscendSort)
			{
				// ���ҵ�һ�������Լ��Ķ���
				if (nResult<0)
				{
					pInsertAfter = pEnumItem->GetPrevItem();
					break;
				}
			}
			else if(bDescendSort)
			{
				// ���ҵ�һ��С���Լ��Ķ��� 
				if(nResult>0)
				{
					pInsertAfter = pEnumItem->GetPrevItem();
					break;
				}
			}

			pEnumItem = pEnumItem->GetNextItem();
		}
	}

	// ����
	return this->InsertItem(pItem, pInsertAfter, nAddItemFlags);
}

// ����ÿһ��ITEM����������
void  ListCtrlBase::UpdateItemIndex(ListItemBase* pStart)
{
#if 0  // ���Ӷ���汾
	if (NULL == pStart)
		pStart = m_pFirstItem;

	ListItemBase* p = pStart;
	while (p != NULL)
	{
		if (NULL == p->GetPrevItem())   // ��һ��
			p->SetLineIndex(0);
		else
			p->SetLineIndex(p->GetPrevItem()->GetLineIndex()+1);

		p = p->GetNextItem();
	}

#else // ���ṹ�汾

    int nTreeIndex = -1;
    int nLineIndex = -1;
    ListItemBase* pItem = pStart;
    if (NULL == pItem)
        pItem = m_pFirstItem;

    if (pItem)
    {
        ListItemBase*  pPrev = pItem->GetPrevTreeItem();
        if (pPrev)
            nTreeIndex = pPrev->GetTreeIndex();

        pPrev = pItem->GetPrevVisibleItem();
        if (pPrev)
            nLineIndex = pPrev->GetLineIndex();
    }

    while (pItem)
    {
        pItem->SetTreeIndex(++nTreeIndex);

        if (pItem->IsVisible())
            pItem->SetLineIndex(++nLineIndex);
        else
            pItem->SetLineIndex(-1);

        if (NULL == pItem->GetPrevItem())
            pItem->SetNeighbourIndex(0);
        else
            pItem->SetNeighbourIndex((pItem->GetPrevItem())->GetNeighbourIndex()+1);

        pItem = pItem->GetNextTreeItem();
    }

#endif
}


void ListCtrlBase::LayoutAllItemsAndRedraw()
{
	LayoutItem(m_pFirstItem, true);
}
void ListCtrlBase::LayoutItem(ListItemBase* pStart, bool bRedraw)
{
	SetCalcFirstLastDrawItemFlag();
#if 0
    bool bWidthNotConfiged = false;
    bool bHeightNotConfiged = false;
    if (!m_pLayoutParam)
    {
        bWidthNotConfiged = true;
        bHeightNotConfiged = true;
    }
    else if (m_pLayoutParam && m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
    {
        CanvasLayoutParam* pParam = static_cast<CanvasLayoutParam*>(m_pLayoutParam);
        bWidthNotConfiged = ((pParam->GetConfigLeft() == NDEF || pParam->GetConfigRight() == NDEF) && pParam->GetConfigWidth()==AUTO);
        bHeightNotConfiged = ((pParam->GetConfigTop() == NDEF || pParam->GetConfigBottom() == NDEF) && pParam->GetConfigHeight() == AUTO);
    }

	if (bWidthNotConfiged || bHeightNotConfiged)
	{
		m_pIListCtrlBase->UpdateLayout(bRedraw);
	}
	else
#endif
	{
		SIZE sizeContent = {0,0};
        IListItemBase*  p = NULL;
        if (pStart)
            p = pStart->GetIListItemBase();

		arrange_item(p?p->GetImpl():NULL, &sizeContent);
		
		m_mgrScrollBar.SetScrollRange(sizeContent.cx, sizeContent.cy);

        // ����hoveritem������arrage���item���ܲ�����λ���������
        if (GetHoverItem())
        {
            m_pIListCtrlBase->SetCanRedraw(false);

            POINT pt = {0, 0};
            ::GetCursorPos(&pt);
            ::MapWindowPoints(NULL, m_pIListCtrlBase->GetHWND(), &pt, 1);
            UISendMessage(m_pIListCtrlBase,
                WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

            m_pIListCtrlBase->SetCanRedraw(true);
        }

		if (bRedraw)
			m_pIListCtrlBase->UpdateObject();
	}
}

void ListCtrlBase::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);

	this->SetCalcFirstLastDrawItemFlag();

	SIZE sizeContent = {0,0};
	arrange_item(NULL, &sizeContent);

	// ���¹�����������
	CRect rcClient;
	m_pIListCtrlBase->GetClientRectInObject(&rcClient);

    SIZE sizePage = { rcClient.Width(), rcClient.Height() };
    m_mgrScrollBar.SetScrollPageAndRange(&sizePage, &sizeContent);
}

void  ListCtrlBase::arrange_item(ListItemBase* pFrom, __out SIZE* pContent)
{
	m_MgrLayout.Arrange(NULL, pContent);

	// TODO:
// 	if (m_pEditingItem)
// 	{
// 		CRect  rcOleEditingItem(0,0,0,0);
// 		m_pEditingItem->GetParentRect(&rcOleEditingItem);
// 
// 		m_MgrLayout.Arrange(p, &sizeContent);
// 		long lVisiblePos = 0;
// 		IsItemVisibleInScreenEx(m_pEditingItem, lVisiblePos);
// 		if (lVisiblePos != LISTITEM_VISIBLE)
// 		{
// 			// ������ȫ�ɼ���ֱ��ֹͣ 
// 			this->DiscardEdit();
// 		}
// 		else
// 		{
// 			// ���±༭��λ��
// 			long lRet = UISendMessage(m_pEditingItem->GetIListItemBase(), 
// 				UI_WM_NOTIFY, w, l, UI_WEN_UPDATEEDITPOS);
// 		}
// 	}
// 	else
// 	{
// 		m_MgrLayout.Arrange(p, &sizeContent);
// 	}
}

void ListCtrlBase::update_mouse_mgr_type()
{
    SAFE_DELETE(m_pMKMgr);

    if (m_listctrlStyle.multiple_sel)
    {
        m_pMKMgr = new MultiSelListCtrlMKMgr;
    }
    else if (m_listctrlStyle.popuplistbox)
    {
        m_pMKMgr = new PopupListCtrlMKMgr;
    }
    else if (m_listctrlStyle.menu)
    {
        m_pMKMgr = new MenuMKMgr;
    }
    else
    {
        m_pMKMgr = new SingleSelListCtrlMKMgr;
    }
    m_pMKMgr->SetListCtrlBase(m_pIListCtrlBase->GetUIApplication()->GetImpl(), this);
}

#pragma  region  // selection

bool ListCtrlBase::IsSelected(ListItemBase* pItem)
{
	if (NULL == pItem)
		return false;

// 	if (m_pFirstSelectedItem == pItem || NULL != pItem->GetPrevSelection())
// 		return true;

    return pItem->IsSelected();
}

// ����һ��ѡ����(ֻѡ��һ��)
void ListCtrlBase::SelectItem(ListItemBase* pItem, bool bUpdate, bool bNotify, bool bMakeVisible)
{
	if (NULL == pItem)  
		return;

    // ��λ����ITEM��ȷ����ȫ�ɼ�
	if (m_pFirstSelectedItem == pItem && NULL == m_pFirstSelectedItem->GetNextSelection())
	{
        if (bMakeVisible)
        {
            bool bNeedUpdate = false;
		    this->MakeItemVisible(m_pFirstSelectedItem, &bNeedUpdate);
            if (bNeedUpdate)
			    this->SetInvalidateAllItems();
        }

        if (GetFocusItem() != pItem)
            SetFocusItem(pItem);

		return;
	}

	ListItemBase* pOldSelectoinItem = m_pFirstSelectedItem;
	ClearSelectItem(false);
	m_pFirstSelectedItem = pItem;
    if (pItem)
        pItem->SetSelected(true);

    if (bMakeVisible)
    {
        bool bNeedUpdate = false;
        this->MakeItemVisible(m_pFirstSelectedItem, &bNeedUpdate);
        if (bNeedUpdate)
            this->SetInvalidateAllItems();
    }

    this->SetFocusItem(pItem);

	if (m_pFirstSelectedItem != pOldSelectoinItem)
	{
		this->InvalidateItem(m_pFirstSelectedItem);
		this->InvalidateItem(pOldSelectoinItem);

        if (bNotify)
        {
            FireSelectItemChanged(pOldSelectoinItem);
        }
	}

	if (bUpdate)
		this->Refresh();
}

// ���ڶ�ѡ��ʽ���������ÿһ��ITEM����һ��ѡ����Ϣ������ֻ����Ϣ������״̬��
// ���ⲿ�Լ�ȥ�ж�����Ҫ��״̬
void  ListCtrlBase::FireSelectItemChanged(ListItemBase* pOldSelectoinItem)
{
    // ֪ͨctrl
	UIMSG  msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_LCN_SELCHANGED_SELF;

	if (!m_listctrlStyle.multiple_sel)
	{
        msg.wParam = (WPARAM)(pOldSelectoinItem?pOldSelectoinItem->GetIListItemBase():NULL);
        msg.lParam = (LPARAM)(m_pFirstSelectedItem?m_pFirstSelectedItem->GetIListItemBase():NULL);
	}
	msg.pMsgFrom = m_pIListCtrlBase;

    // �Ƚ������ദ��
    msg.pMsgTo = m_pIListCtrlBase;
    UISendMessage(&msg, 0, 0);

    // ��֪ͨ�ⲿ���� 
    msg.pMsgTo = NULL;
    msg.bHandled = FALSE;
	msg.nCode = UI_LCN_SELCHANGED;
	m_pIListCtrlBase->DoNotify(&msg);
}

ListItemBase* ListCtrlBase::GetLastSelectItem()
{
	ListItemBase* pItem = m_pFirstSelectedItem;
	while (pItem)
	{
		ListItemBase* pNext = pItem->GetNextSelection();
		if (NULL == pNext)
			break;
		
		pItem = pNext;
	}

	return pItem;
}

// �����ǰû��ѡ���ֻ����Ϊѡ�������Ѿ���ѡ���������Ϊ��һ��ѡ����
void ListCtrlBase::AddSelectItem(ListItemBase* pItem, bool bNotify)
{
	if (NULL == pItem)
		return;

	if (m_listctrlStyle.multiple_sel)
	{
		if (pItem->IsSelected())
			return;

		ListItemBase* pLastSelItem = this->GetLastSelectItem();
        if (pLastSelItem)
        {
            pLastSelItem->SetNextSelection(pItem);
            pItem->SetPrevSelection(pLastSelItem);
        }
        else
        {
            m_pFirstSelectedItem = pItem;
        }

        pItem->SetSelected(true);

        if (NULL == GetFocusItem())
            this->SetFocusItem(pItem);

        this->InvalidateItem(pItem);

        if (bNotify)
        {
            this->FireSelectItemChanged(NULL);
        }
	}
	else   // ��ѡ
	{
		if (m_pFirstSelectedItem == pItem)
			return;

		SelectItem(pItem, false, bNotify);
	}	
}
void ListCtrlBase::RemoveSelectItem(ListItemBase* pItem, bool bNotify)
{
	if (NULL == pItem || NULL == m_pFirstSelectedItem)
		return;

	if (!pItem->IsSelected())
		return;

	ListItemBase* pOldSelection = m_pFirstSelectedItem;
	if (pItem->GetPrevSelection())
		pItem->GetPrevSelection()->SetNextSelection(pItem->GetNextSelection());
	if (pItem->GetNextSelection())
		pItem->GetNextSelection()->SetPrevSelection(pItem->GetPrevSelection());

    if (m_pFirstSelectedItem == pItem)
        m_pFirstSelectedItem = pItem->GetNextSelection();

	pItem->SetPrevSelection(NULL);
	pItem->SetNextSelection(NULL);

    pItem->SetSelected(false);
	if (bNotify)
		this->FireSelectItemChanged(pOldSelection);
}
void ListCtrlBase::ClearSelectItem(bool bNotify)
{
	if (NULL == m_pFirstSelectedItem)
		return;

	ListItemBase* pOldSelection = m_pFirstSelectedItem;
	
	ListItemBase* pItem = m_pFirstSelectedItem;
	while (pItem)
	{
		this->InvalidateItem(pItem);
        pItem->SetSelected(false);

		ListItemBase* pNextItem = pItem->GetNextSelection();
		if (NULL == pNextItem)
			break;

		pNextItem->SetPrevSelection(NULL);
		pItem->SetNextSelection(NULL);
		pItem = pNextItem;
	}
	m_pFirstSelectedItem = NULL;

	if (bNotify)
	{
		this->FireSelectItemChanged(pOldSelection);
	}
}
UINT  ListCtrlBase::GetSelectedItemCount()
{
    UINT  nCount = 0;
    ListItemBase* p = m_pFirstSelectedItem;
    while (p)
    {
        nCount ++;
        p = p->GetNextSelection();
    }
    return nCount;
}

// �Ƿ�ѡȡ�˶��������1����
bool  ListCtrlBase::IsSelectMulti()
{
    if (!m_pFirstSelectedItem)
        return false;
    if (m_pFirstSelectedItem->GetNextSelection())
        return true;

    return false;
}

void  ListCtrlBase::SelectAll(bool bUpdate)
{
    ClearSelectItem(false);
    ListItemBase*  p = FindSelectableItemFrom(NULL);
    m_pFirstSelectedItem = p;

    ListItemBase*  pNext = NULL;
    while (p)
    {
        p->SetSelected(true);

        pNext = p->GetNextSelectableItem();
        p->SetNextSelection(pNext);
        if (pNext)
            pNext->SetPrevSelection(p);

        p = pNext;
    }

    this->FireSelectItemChanged(NULL);

    if (bUpdate)
        m_pIListCtrlBase->UpdateObject();
    else
        this->SetInvalidateAllItems();
}
#pragma endregion

ListItemBase* ListCtrlBase::GetHoverItem()
{
    if (NULL == m_pMKMgr)
        return NULL;

    return m_pMKMgr->GetHoverItem();
}
ListItemBase* ListCtrlBase::GetPressItem()
{
    if (NULL == m_pMKMgr)
        return NULL;

    return m_pMKMgr->GetPressItem();
}

ListItemBase* ListCtrlBase::GetFocusItem()
{
    if (NULL == m_pMKMgr)
        return NULL;

    return m_pMKMgr->GetFocusItem();
}
void  ListCtrlBase::SetFocusItem(ListItemBase* pItem)
{
    m_pMKMgr->SetFocusItem(pItem);
    return;
// 
// 	UIASSERT(0 && _T("focus�ѷŵ�mkmgr��ʵ�� "));
// 	if (m_pFocusItem == pItem)
// 		return;
// 
// 	if (m_pFocusItem)
// 		this->InvalidateItem(m_pFocusItem);
// 
//     if (m_pFocusItem)
//         m_pFocusItem->SetFocus(false);
// 
// 	m_pFocusItem = pItem;
// 
//     if (m_pFocusItem)
//         m_pFocusItem->SetFocus(true);
// 
// 	if (m_pFocusItem)
// 		this->InvalidateItem(m_pFocusItem);
}

Object*  ListCtrlBase::GetHoverObject()
{
    return m_pMKMgr->GetHoverObject();
}
Object*  ListCtrlBase::GetPressObject()
{
    return m_pMKMgr->GetPressObject();
}
void  ListCtrlBase::SetFocusObject(Object* pObj)
{
    m_pMKMgr->SetFocusObject(pObj);
}
Object*  ListCtrlBase::GetFocusObject()
{
    return m_pMKMgr->GetFocusObject();
}
SIZE ListCtrlBase::GetAdaptWidthHeight(int nWidth, int nHeight)
{
	SIZE s = {nWidth,nHeight};

	if (m_sizeMin.cx != NDEF && nWidth < m_sizeMin.cx)
		s.cx = m_sizeMin.cx;
	if (m_sizeMax.cx != NDEF && nWidth > m_sizeMax.cx)
		s.cx = m_sizeMax.cx;

	if (m_sizeMin.cy != NDEF && nHeight < m_sizeMin.cy)
		s.cy = m_sizeMin.cy;
	if (m_sizeMax.cy != NDEF && nHeight > m_sizeMax.cy)
		s.cy = m_sizeMax.cy;

	return s;
}

void ListCtrlBase::Scroll2Y(int nY, bool bUpdate)
{
	this->SetScrollPos(-1, nY, bUpdate);
}
void ListCtrlBase::Scroll2X(int nX, bool bUpdate)
{
	this->SetScrollPos(nX, -1, bUpdate);
}
void  ListCtrlBase::ScrollY(int nY, bool bUpdate)
{
    this->SetScrollPos(-1, m_mgrScrollBar.GetVScrollPos() + nY, bUpdate);
}
void  ListCtrlBase::ScrollX(int nX, bool bUpdate)
{
    this->SetScrollPos(m_mgrScrollBar.GetHScrollPos() + nX, -1, bUpdate);
}
void ListCtrlBase::SetScrollPos(int nX, int nY, bool bUpdate)
{
    int nOldX = m_mgrScrollBar.GetHScrollPos();
    int nOldY = m_mgrScrollBar.GetVScrollPos();
    
    if (-1 != nX)
    {
        m_mgrScrollBar.SetHScrollPos(nX);        
    }
    if (-1 != nY)
    {
        m_mgrScrollBar.SetVScrollPos(nY);
    }

    if (nOldX != m_mgrScrollBar.GetHScrollPos() ||
        nOldY != m_mgrScrollBar.GetVScrollPos())
    {
        SetCalcFirstLastDrawItemFlag();
        if (bUpdate)
            m_pIListCtrlBase->UpdateObject();
    }
}

bool ListCtrlBase::InsertItem(ListItemBase* pItem, UINT nPos, int nAddItemFlags)
{
	ListItemBase* pInsertAfter = NULL;
	if (nPos >= m_nItemCount)
		pInsertAfter = m_pLastItem;
	else
		pInsertAfter = this->GetItemByPos(nPos, true);

	return this->InsertItem(pItem, pInsertAfter, nAddItemFlags);
}
//
// ��pInsertAfterǰ�����pItem�����pInsertAfterΪNULL��ʾ������ǰ��
//
bool ListCtrlBase::InsertItem(ListItemBase*  pItem, ListItemBase* pInsertAfter, int nAddItemFlags)
{
	if (NULL == pItem)
		return false;
	
	if (false == this->_InsertItem(pItem, pInsertAfter))
		return false;

    pItem->SetIListCtrlBase(m_pIListCtrlBase);
    UISendMessage(pItem->GetIListItemBase(), UI_WM_INITIALIZE);

    if (nAddItemFlags & LF_UPDATEITEMINDEX)
    {
	    this->UpdateItemIndex(pItem);
    }
	if (nAddItemFlags & LF_LAYOUT)
	{
		this->LayoutItem(pItem, false);
	}

    if (nAddItemFlags & LF_REDRAW)
    {
        m_pIListCtrlBase->UpdateObject();
    }

    UIMSG  msg;
    msg.pMsgTo = m_pIListCtrlBase;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_LCN_ITEMADD;
    msg.wParam = (WPARAM)pItem->GetIListItemBase();
    msg.lParam = nAddItemFlags;
    UISendMessage(&msg);

	return true;
}


// ��InsertItem���á��������ݲ���Ĺ�����������SIZE
bool ListCtrlBase::_InsertItem(ListItemBase* pItem, ListItemBase* pInsertAfter)
{
	if (NULL == pItem)
		return false;

    // TODO: �������� IListItemBase������Ĳ�����ListItemBase�����ô���
//     if (pInsertAfter == UITVI_FIRST || pInsertAfter == UITVI_ROOT)
//     {
//         pInsertAfter = NULL;
//     }
//     else if (pInsertAfter == UITVI_LAST)
//     {
//         pInsertAfter = m_pLastItem;
//     }

	if (NULL == pInsertAfter)// ������λ
	{
		if (NULL == m_pFirstItem)
		{
			m_pFirstItem = m_pLastItem = pItem;
		}
		else
		{
			m_pFirstItem->SetPrevItem(pItem);
			pItem->SetNextItem(m_pFirstItem);
			m_pFirstItem = pItem;
		}
	}
	else
	{
		if (NULL == pInsertAfter->GetNextItem())
		{
			pInsertAfter->SetNextItem(pItem);
			pItem->SetPrevItem(pInsertAfter);
			m_pLastItem = pItem;
		}
		else
		{
			pInsertAfter->GetNextItem()->SetPrevItem(pItem);
			pItem->SetNextItem(pInsertAfter->GetNextItem());
			pInsertAfter->SetNextItem(pItem);
			pItem->SetPrevItem(pInsertAfter);
		}
	}
    
    pItem->SetIListCtrlBase(m_pIListCtrlBase);

	m_nItemCount++;
    if (pItem->GetId() != 0)
    {
        m_mapItem[pItem->GetId()] = pItem->GetIListItemBase();
    }
	SetCalcFirstLastDrawItemFlag();

	return true;
}

void  ListCtrlBase::GetDesiredSize(SIZE* pSize) 
{
    m_MgrLayout.Measure(pSize);
}


#if 0 // -- �ܹ�����  -- ����
// ��ȡ����item�У����Ŀ��ֵ(���pDesiredHeight��Ϊ�գ���ͬʱ�����ܹ���Ҫ�ĸ߶�)
int ListCtrlBase::GetMaxDesiredWidth(int* pDesiredHeight)
{
	if (pDesiredHeight)
		*pDesiredHeight = 0;

	int nRet = 0;
	ListItemBase* p = m_pFirstItem;
	while (p != NULL)
	{
		SIZE s = GetItemDesiredSize(p);
		if (s.cx > nRet)
			nRet = s.cx;

		if (pDesiredHeight)
		{
			*pDesiredHeight += s.cy;
			if (p != m_pFirstItem)
			{
				*pDesiredHeight += m_nVertSpacing;
			}
		}
		p = p->GetNextVisibleItem();
	}

	return nRet;
}
#endif

void  ListCtrlBase::SetCalcFirstLastDrawItemFlag()
{
    m_bNeedCalcFirstLastDrawItem = true; 
    m_pFirstDrawItem = m_pLastDrawItem = NULL;
}

// ���㵱ǰ�ĵ�һ������ITEM
void  ListCtrlBase::CalcFirstLastDrawItem()
{
	m_pFirstDrawItem = NULL;
	m_pLastDrawItem = NULL;

	CRect  rcClient;
	m_pIListCtrlBase->GetObjectClientRect(&rcClient);

	int xOffset = 0, yOffset = 0;
    m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);

    ListItemBase* pItem = FindVisibleItemFrom(m_pFirstItem);
    while (pItem)
    {
        CRect  rcParent;
        pItem->GetParentRect(&rcParent);
        if (rcParent.bottom - yOffset <= rcClient.top)  // top cover unvisible item
        {
            pItem = pItem->GetNextVisibleItem();
            continue;
        }
        if (rcParent.right - xOffset <= rcClient.left)
        {
            pItem = pItem->GetNextVisibleItem();
            continue;
        }

        if (NULL == m_pFirstDrawItem)
            m_pFirstDrawItem = pItem;

        if (rcParent.top - yOffset >= rcClient.bottom)  // last visible item
            break;
        if (rcParent.left - xOffset >= rcClient.right)
            break;

        m_pLastDrawItem = pItem;
        pItem = pItem->GetNextVisibleItem();
    }

    if (m_listctrlStyle.float_group_head)
        UISendMessage(m_pIListCtrlBase, UI_LCN_FIRSTLASTDRAWITEM_UPDATED);
}

ListItemBase*  ListCtrlBase::GetFirstDrawItem()
{
	return m_pFirstDrawItem;   // �п������ʱ��m_nNeedCalcFirstLastVisibleItemΪtrue������Ȼ����m_pFirstVisibleItem����Ϊ���ܻ�û�е���UpdateRectItem
}
ListItemBase*  ListCtrlBase::GetLastDrawItem()
{
	return m_pLastDrawItem;
}
ListItemBase*  ListCtrlBase::FindVisibleItemFrom(ListItemBase* pFindFrom)
{
    if (NULL == pFindFrom)
        pFindFrom = m_pFirstItem;

    while (pFindFrom)
    {
        if (pFindFrom->IsVisible())
            return pFindFrom;

        pFindFrom = pFindFrom->GetNextVisibleItem();
    }

    return NULL;
}
ListItemBase*  ListCtrlBase::FindSelectableItemFrom(ListItemBase* pFindFrom)
{
    if (NULL == pFindFrom)
        pFindFrom = m_pFirstItem;

    while (pFindFrom)
    {
        if (pFindFrom->IsSelectable())
            return pFindFrom;

        pFindFrom = pFindFrom->GetNextSelectableItem();
    }

    return NULL;
}

ListItemBase*  ListCtrlBase::FindFocusableItemFrom(ListItemBase* pFindFrom)
{
    if (NULL == pFindFrom)
        pFindFrom = m_pFirstItem;

    while (pFindFrom)
    {
        if (pFindFrom->IsFocusable())
            return pFindFrom;

        pFindFrom = pFindFrom->GetNextFocusableItem();
    }

    return NULL;
}

bool ListCtrlBase::IsItemVisibleInScreen(ListItemBase* pItem)
{
	if (NULL == pItem)
		return false;

	if (false == pItem->IsVisible())
		return false;

	CRect rcClient;
	m_pIListCtrlBase->GetClientRectInObject(&rcClient);

	int xOffset = 0, yOffset = 0;
	m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);


    CRect rcParent;
    pItem->GetParentRect(&rcParent);
    if (rcParent.bottom - yOffset <= 0) 
        return false;

    if (rcParent.top - yOffset >= rcClient.Height())  
        return false;

    if (rcParent.left - xOffset >= rcClient.Width())
        return false;

    if (rcParent.right - xOffset <= 0)
        return false;

    return true;
}
bool ListCtrlBase::IsItemVisibleInScreenEx(
    ListItemBase* pItem,
    /*LISTITEM_VISIBLE_POS_TYPE*/long& ePos)
{
    ePos = LISTITEM_UNVISIBLE_ERROR;

	if (NULL == pItem)
		return false;

	if (false == pItem->IsVisible())
		return false;

    ePos = LISTITEM_VISIBLE;

	CRect rcClient, rcItemParent;
	m_pIListCtrlBase->GetClientRectInObject(&rcClient);
	pItem->GetParentRect(&rcItemParent);

	int xOffset = 0, yOffset = 0;
	m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);

	int yTop = rcItemParent.top - yOffset;
	int yBottom = rcItemParent.bottom - yOffset;
    int xLeft = rcItemParent.left - xOffset;
    int xRight = rcItemParent.right - xOffset;

	if (yBottom <= 0) 
	{
		ePos = LISTITEM_UNVISIBLE_TOP;
		return false;
	}
	else if (yTop >= rcClient.Height())
	{
		ePos = LISTITEM_UNVISIBLE_BOTTOM;
		return false;
	}
    else if (xRight <= 0)
    {
        ePos = LISTITEM_UNVISIBLE_LEFT;
        return false;
    }
    else if (xLeft >= rcClient.Width())
    {
        ePos = LISTITEM_UNVISIBLE_RIGHT;
        return false;
    }

    if (yTop < 0)
    {
        ePos |= LISTITEM_VISIBLE_COVERTOP;
    }
    if (yBottom > rcClient.Height())
    {
        ePos |= LISTITEM_VISIBLE_COVERBOTTOM;
    }
    if (xLeft < 0)
    {
        ePos |= LISTITEM_VISIBLE_COVERLEFT;
    }
    if (xRight > rcClient.Width())
    {
        ePos |= LISTITEM_VISIBLE_COVERRIGHT;
    }

	return true;
}

// ���ù�������λ�ã�ȷ��pItem�ɼ�
// ���ؿؼ��Ƿ�����ˣ��Ƿ���Ҫˢ�£�
void  ListCtrlBase::MakeItemVisible(ListItemBase* pItem, bool* pbNeedUpdate)
{
	bool  bNeedUpdateObject = false;

	if (NULL == pItem)
		return ;

	/*LISTITEM_VISIBLE_POS_TYPE*/long ePosType = LISTITEM_VISIBLE;
	this->IsItemVisibleInScreenEx(pItem, ePosType);
    if (ePosType == LISTITEM_VISIBLE ||
        ePosType == LISTITEM_UNVISIBLE_ERROR)
        return;

    CRect rc;
    CRect rcClient;
    pItem->GetParentRect(&rc);
    m_pIListCtrlBase->GetClientRectInObject(&rcClient);

	if (LISTITEM_UNVISIBLE_TOP & ePosType || LISTITEM_VISIBLE_COVERTOP & ePosType)
	{
		bNeedUpdateObject = true;
		m_mgrScrollBar.SetVScrollPos(rc.top);
	}
	else if (LISTITEM_UNVISIBLE_BOTTOM & ePosType || LISTITEM_VISIBLE_COVERBOTTOM & ePosType)
	{
		bNeedUpdateObject = true;
		m_mgrScrollBar.SetVScrollPos(rc.bottom - rcClient.Height());
	}

    if (ePosType & LISTITEM_UNVISIBLE_LEFT || ePosType & LISTITEM_VISIBLE_COVERLEFT)
    {
        // ����һ����ʾ����һ�е����
        int nLimitScrollPos = rc.right - rcClient.Width();
        int nDesiredScrollPos = rc.left;
        if (nDesiredScrollPos > nLimitScrollPos)
        {
            bNeedUpdateObject = true;
            m_mgrScrollBar.SetHScrollPos(nDesiredScrollPos);
        }
    }
    else if (ePosType & LISTITEM_UNVISIBLE_RIGHT || ePosType & LISTITEM_VISIBLE_COVERRIGHT)
    {
        // ����һ����ʾ����һ�е����
        int nLimitScrollPos = rc.left;
        int nDesiredScrollPos = rc.right - rcClient.Width();
        if (nDesiredScrollPos < nLimitScrollPos)
        {
            bNeedUpdateObject = true;
            m_mgrScrollBar.SetHScrollPos(nDesiredScrollPos);
        }
    }

    if (bNeedUpdateObject)
        this->SetCalcFirstLastDrawItemFlag();

    if (pbNeedUpdate)
        *pbNeedUpdate = bNeedUpdateObject;
}


void  ListCtrlBase::ObjectRect2WindowRect(RECT* prcObj, RECT* prcWindow)
{
    if (!prcObj || !prcWindow)
        return;

    RECT  rcWnd = *prcObj;

    RECT rcCtrl;
    m_pIListCtrlBase->GetClientRectInWindow(&rcCtrl);
    OffsetRect(&rcWnd, rcCtrl.left, rcCtrl.top);

    CopyRect(prcWindow, &rcWnd);
}

void  ListCtrlBase::ItemRect2ObjectRect(RECT* prcItem, RECT* prcObject)
{
    if (!prcItem || !prcObject)
        return;

    RECT  rcObj = *prcItem;

    int xOffset = 0, yOffset = 0;
    m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);
    OffsetRect(&rcObj, -xOffset, -yOffset);

    CopyRect(prcObject, &rcObj);
}

void ListCtrlBase::ItemRect2WindowRect(LPCRECT prc, LPRECT prcRet)
{
	if (NULL == prc || NULL == prcRet)
		return;

	CRect rcItem(*prc);

	int xOffset = 0, yOffset = 0;
	m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);
	rcItem.OffsetRect(-xOffset, -yOffset);

	CRect rcWindow;
	m_pIListCtrlBase->GetClientRectInWindow(&rcWindow);
	rcItem.OffsetRect(rcWindow.left, rcWindow.top);

	IntersectRect(prcRet, &rcItem, &rcWindow);
}


void ListCtrlBase::WindowPoint2ItemPoint(ListItemBase* pItem, const POINT* ptWnd, POINT* ptItem)
{
    if (NULL == pItem || NULL == ptWnd || NULL == ptItem)
        return;

    if (pItem->IsFloat())
    {
        this->WindowPoint2ObjectPoint(ptWnd, ptItem, true);

        RECT  rc;
        pItem->GetFloatRect(&rc);
        ptItem->x -= rc.left;
        ptItem->y -= rc.top;
    }
    else
    {
        CRect rcItem;
        pItem->GetParentRect(&rcItem);

        m_pIListCtrlBase->WindowPoint2ObjectClientPoint(ptWnd, ptItem, true);
        ptItem->x -= rcItem.left;
        ptItem->y -= rcItem.top;
    }
}

void ListCtrlBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // ���ⲿ����notify������ⲿ�������ٽ���mousekeymgr����
	UIMSG  msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_NM_KEYDOWN;
	msg.wParam = nChar;
	msg.pMsgFrom = m_pIListCtrlBase;

    long lRet = m_pIListCtrlBase->DoNotify(&msg);
	if (0 == lRet)
	{
		SetMsgHandled(FALSE);
	}
}

void ListCtrlBase::OnStateChanged(UINT nMask)
{
    SetMsgHandled(FALSE);
	if (nMask & OSB_FOCUS)
	{
		m_pIListCtrlBase->UpdateObject();
	}
}

void ListCtrlBase::OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
	SetMsgHandled(FALSE);
	SetCalcFirstLastDrawItemFlag();
}
void ListCtrlBase::OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
    SetMsgHandled(FALSE);
    SetCalcFirstLastDrawItemFlag();
}
BOOL  ListCtrlBase::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
    BOOL bHandled = TRUE;
    BOOL bNeedRefresh = FALSE;
    m_mgrScrollBar.DoMouseWheel(nFlags, zDelta, pt, bHandled, bNeedRefresh);

    if (bNeedRefresh)
    {
        SetCalcFirstLastDrawItemFlag();

        SetInvalidateAllItems();
        Refresh();
    }

    return bHandled;
}

LRESULT  ListCtrlBase::OnInertiaVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SetCalcFirstLastDrawItemFlag();
    this->UpdateObject();
    return 0;
}

//
// ע��ÿһ��ITEM�Ļ��Ʋ�����ȥ����Context������ֱ��ʹ��m_rcParent��������ؼ�һ���л����ؼ������Ͻ�ʹ�ã�0��0������
//     ����ڻ���ITEM�ڲ��ؼ�ʱ��Ҫȥ����context
//
void ListCtrlBase::OnPaint(IRenderTarget* pRenderTarget, RenderContext* pContext)
{ 
    m_bPaintingCtrlRef ++;
	if (m_bNeedCalcFirstLastDrawItem)
	{
		m_bNeedCalcFirstLastDrawItem = false;
		this->CalcFirstLastDrawItem();
	}

#if 0
    ISelectRegionRender* pSelectRegionRender = m_pMouseMgr->GetSelectRegionRender();
    if (pSelectRegionRender)
    {
        pSelectRegionRender->PrePaint();
    }
#endif

    // ˢ����Ч��
    if (m_bRedrawInvalidItems)
    {
        UIASSERT( GetInvalidateItemCount() > 0);  
        for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
        {
            if (!m_pInvalidateItems[i])
                continue;

            if (HANDLED == this->OnDrawItem(pRenderTarget, m_pInvalidateItems[i]))
                continue;
            m_pInvalidateItems[i]->Draw(pRenderTarget, pContext);
            m_pInvalidateItems[i]->DrawItemInnerControl(pRenderTarget, pContext);
        }
    }
    // ˢ��������
    else
    {
	    ListItemBase* pItem = m_pFirstDrawItem;
	    while (pItem)
	    {
		    if (HANDLED == this->OnDrawItem(pRenderTarget,pItem))             // ���Ʊ���
                continue;

            pItem->Draw(pRenderTarget, pContext);                             // �Ӷ������
		    pItem->DrawItemInnerControl(pRenderTarget, pContext);             // �����ڲ��ؼ�

		    if (pItem == m_pLastDrawItem)
			    break;

		    pItem = pItem->GetNextVisibleItem();
	    }
    }

    // ����������clear���п��������������ˢ������ģ�������_redrawitem����
    // this->ClearInvalidateItems();

    m_bPaintingCtrlRef--;
}

void  ListCtrlBase::OnPostPaint(IRenderTarget* pRenderTarget)
{
    m_MgrFloatItem.DoPaint(pRenderTarget);
}

HANDLED_VALUE  ListCtrlBase::OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p)
{
	ControlStyle s = {0};
	s.ownerdraw = 1;
    if (m_pIListCtrlBase->TestControlStyle(&s))
    {
        OWNERDRAWSTRUCT s;
        s.pItemDraw = p->GetIListItemBase();
        s.pObjDraw = m_pIListCtrlBase;
        s.pRenderTarget = pRenderTarget;

        UIMSG  msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_WM_OWNERDRAW;
        msg.wParam = (WPARAM)&s;
        msg.pMsgFrom = m_pIListCtrlBase;

        if (HANDLED == m_pIListCtrlBase->DoNotify(&msg))
            return HANDLED;
    }

    ListItemStyle s2 = {0};
    s2.bOwnerDraw = 1;
    if (p->TestStyle(s2))
    {
        OWNERDRAWSTRUCT s;
        s.pItemDraw = p->GetIListItemBase();
        s.pObjDraw = m_pIListCtrlBase;
        s.pRenderTarget = pRenderTarget;

        UIMSG  msg;
        msg.message = UI_WM_OWNERDRAW;
        msg.wParam = (WPARAM)&s;
        msg.pMsgFrom = msg.pMsgTo = m_pIListCtrlBase;
        if (HANDLED == UISendMessage(&msg))
            return HANDLED;
    }

    return NOT_HANDLED;
}

void ListCtrlBase::SetHoverItem(ListItemBase* pItem)
{
	if (m_pMKMgr)
        m_pMKMgr->SetHoverItem(pItem);
}
void ListCtrlBase::SetPressItem(ListItemBase* pItem)
{
	if (m_pMKMgr)
        m_pMKMgr->SetPressItem(pItem);
}


// void ListCtrlBase::OnFontModifyed(IRenderFont* pFont)
// {
// 	if (pFont == GetRenderFont() && NULL != pFont)
// 	{
// 		ListItemBase* pItem = m_pFirstItem;
// 		while (pItem)
// 		{
// 			this->MeasureItem(pItem);
// 			pItem = pItem->GetNextItem();
// 		}
// 	}
// 
// 	this->LayoutItem();
// }

#pragma region // Invalidate Item
void  ListCtrlBase::InvalidateItem(ListItemBase* pItem)
{
	if (NULL == pItem)
		return;

	if (m_eInvalidateFlag == LISTCTRL_NEED_UPDATE_FLAG_ALL)
		return;

    CRect rc;
    pItem->GetParentRect(&rc);
    if (rc.IsRectEmpty())
        return;
    if (!pItem->IsVisible())
        return;
	
	// ����direct2d��֧��ͬʱ���ö�����ཻ�ļ������������ʹ��
	// direct2d�����ʱ�����������1����Ч���ֱ��ˢ�������ؼ�
	int  nMaxInvalidateItemCount = LISTCTRL_MAX_INVALIDATE_ITEM_COUNT;
	WindowRender* pWindowRender = GetWindowRender();
	if (pWindowRender)
	{
		GRAPHICS_RENDER_LIBRARY_TYPE eType = pWindowRender->GetGraphicsRenderType();
		if (GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D == eType ||
			GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D == eType)
		{
			nMaxInvalidateItemCount = 1;
		}
	}
	for (int i = 0; i < nMaxInvalidateItemCount; i++)
	{
		if (m_pInvalidateItems[i] == pItem)
		{
			return;  // �Ѿ�����
		}
		else if (m_pInvalidateItems[i] == NULL)
		{
			m_pInvalidateItems[i] = pItem;
			m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_ITEM;
			return;
		}
	}

	// ����
	m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_ALL;
	for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
		m_pInvalidateItems[i] = NULL;
}

// pItem��ɾ��ʱ������(�����ӽ��)����Ч�б����Ƴ�
void  ListCtrlBase::RemoveInvavlidateItem(ListItemBase* pItem)
{
    if (NULL == pItem)
        return;

    if (m_eInvalidateFlag == LISTCTRL_NEED_UPDATE_FLAG_ALL ||
        m_eInvalidateFlag == LISTCTRL_NEED_UPDATE_FLAG_NONE)
        return;

    for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
    {
        if (!m_pInvalidateItems[i])
            continue;

        if (m_pInvalidateItems[i] == pItem)
        {
            m_pInvalidateItems[i] = NULL;
        }
        else if (pItem->IsMyChildItem(m_pInvalidateItems[i], true))
        {
            m_pInvalidateItems[i] = NULL;
        }
    }

    // ����
    if (0 == GetInvalidateItemCount())
    {
        m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_NONE;
    }
}

void ListCtrlBase::SetInvalidateAllItems()
{
	this->ClearInvalidateItems();
	m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_ALL;
}

// �����Ч�������OnPaint֮ǰ
void  ListCtrlBase::ClearInvalidateItems()
{
	if (LISTCTRL_NEED_UPDATE_FLAG_NONE != m_eInvalidateFlag)
	{
		m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_NONE;
		for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
			m_pInvalidateItems[i] = NULL;
	}
}

int  ListCtrlBase::GetInvalidateItemCount()
{
	int nCount = 0;
	for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
	{
		if (m_pInvalidateItems[i] != NULL)
			nCount++;
	}

	return nCount;
}

// ˢ��invalidate item
void  ListCtrlBase::Refresh()
{
	if (m_eInvalidateFlag == LISTCTRL_NEED_UPDATE_FLAG_ALL)
	{
        m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_NONE;

        if (!m_bPaintingCtrlRef)
		    m_pIListCtrlBase->UpdateObject();
	}
	else if (m_eInvalidateFlag == LISTCTRL_NEED_UPDATE_FLAG_NONE)
	{
		return;
	}
	else
	{
        if (!m_bPaintingCtrlRef)
		    this->redraw_item(m_pInvalidateItems, LISTCTRL_MAX_INVALIDATE_ITEM_COUNT);
	}
}

// �ڲ��ؼ���ˢ��ʱ����RedrawItem��Щ��ͬ��
// 1. ��ʱ���л�ȡ��pRenderTarget�ˣ�����Ҫ����begindrawpart. <<-- �����ڻ�����һЩ���Ʊ����Ĳ���
// 2. ����Ҫcommit����������
// 3. ����Ҫ�ٻ���innerctrl��ֻ��Ҫlistitem����
void ListCtrlBase::RedrawItemByInnerCtrl(IRenderTarget* pRenderTarget, RenderContext* pContext, ListItemBase* pItem)
{
	if (!IsItemVisibleInScreen(pItem))
		return;

    if (HANDLED == this->OnDrawItem(pRenderTarget, pItem))
        return;

    pItem->Draw(pRenderTarget, pContext); 
}


void  ListCtrlBase::redraw_item(ListItemBase** ppItemArray, int nCount)
{
	if (NULL == ppItemArray && 0 == nCount)
		return;

    int nRealCount = 0;
    CRect* prcObjArray = new CRect[nCount];
    for (int i = 0; i < nCount; i++)
    {
        if (ppItemArray[i])
        {
            if (ppItemArray[i]->IsFloat())
            {
                RECT rcFloat;
                ppItemArray[i]->GetFloatRect(&rcFloat);
                if (IsRectEmpty(&rcFloat))
                    continue;

                ObjectRect2WindowRect(&rcFloat, &prcObjArray[nRealCount]);
                nRealCount++;
            }
            else
            {
                CRect rc;
                ppItemArray[i]->GetParentRect(&rc);
                if (rc.IsRectEmpty())
                    continue;

                ObjectClientRect2ObjectRect(this, 
                    ppItemArray[i]->GetParentRectPtr(),
                    &prcObjArray[nRealCount]);
                nRealCount++;
            }
        }
    }
    if (nRealCount)
    {
        m_bRedrawInvalidItems = true;
        UpdateObjectEx(prcObjArray, nRealCount, true);  // ���ᴥ��OnPaint
        m_bRedrawInvalidItems = false;

        this->ClearInvalidateItems();  
    }
    SAFE_ARRAY_DELETE(prcObjArray);
}
#pragma endregion


//////////////////////////////////////////////////////////////////////////

LPCTSTR  ListCtrlBase::GetItemText(ListItemBase* pItem)
{
	if (NULL == pItem)
		return NULL;
	
	return pItem->GetText();
}

int  ListCtrlBase::GetVisibleItemCount()
{
    int nRet = 0;
    ListItemBase*  pItem = FindVisibleItemFrom(NULL);
    while (pItem)
    {
        nRet ++;
        pItem = pItem->GetNextVisibleItem();
    }
    return nRet;
}
int   ListCtrlBase::GetRootItemCount()
{
    int i = 0;

    ListItemBase* p = m_pFirstItem;
    while (p)
    {
        i++;
        p = p->GetNextItem();
    }

    return i;
}

// item share data op
IListItemTypeShareData*  ListCtrlBase::GetItemTypeShareData(int lType)
{
    map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.find(lType);
    if (iter != m_mapItemTypeShareData.end())
        return iter->second;

    return NULL;
}
void  ListCtrlBase::SetItemTypeShareData(int lType, IListItemTypeShareData* pData)
{
    m_mapItemTypeShareData[lType] = pData;
    
    // ��������
    if (pData)
    {
        pData->GetImpl()->SetListCtrlBase(this);
        if (this->m_pIMapAttributeRemain)
        {
			SERIALIZEDATA data = {0};
            data.pUIApplication = GetIUIApplication();
			data.pMapAttrib = m_pIMapAttributeRemain;
			data.nFlags = SERIALIZEFLAG_LOAD;
			UISendMessage(pData, UI_WM_SERIALIZE, (WPARAM)&data);
//          UISendMessage(pData, UI_WM_SETATTRIBUTE, (WPARAM)this->m_pIMapAttributeRemain, 0);
        }
    }
}
void  ListCtrlBase::RemoveItemTypeShareData(int lType)
{
    map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.find(lType);
    if (iter != m_mapItemTypeShareData.end())
    {
        m_mapItemTypeShareData.erase(iter);
    }
}

// ptΪ��������
// ListItemBase* ListCtrlBase::HitTest(POINT ptWindow)
// {
//     return GetItemByWindowPoint(ptWindow);
//     
//     CRect rcClient;
//     m_pIListCtrlBase->GetClientRectInObject(&rcClient);
// 
//     // 1. ת��Ϊ�ڲ�����
// 
//     POINT pt;
//     m_pIListCtrlBase->WindowPoint2ObjectPoint(&ptWindow, &pt, true);
//     if (FALSE == rcClient.PtInRect(pt))
//         return NULL;
// 
//     m_pIListCtrlBase->ObjectPoint2ObjectClientPoint(&pt, &pt);
// 
//     // 2. �ж�
// 
//     ListItemBase* p = m_pFirstDrawItem;
//     while (p)
//     {
//         CRect rcParent;
//         p->GetParentRect(&rcParent);
//         if (PtInRect(&rcParent, pt))
//         {
//             if (ptItem)
//             {
//                 ptItem->x = pt.x - rcParent.left;
//                 ptItem->y = pt.y - rcParent.top;
//             }
//             return p;
//         }
// 
//         if (p == m_pLastDrawItem)
//             break;
// 
//         p = p->GetNextVisibleItem();
//     }
// 
//     return NULL;
// }

// ptΪ�ؼ��ڲ�����
LRESULT  ListCtrlBase::OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT pt = {wParam, lParam};

    // 1. ���float item 
    ListItemBase* pItem = m_MgrFloatItem.HitTest(pt);
    if (pItem)
        return (LRESULT)pItem->GetIListItemBase();

    // 2. ������item
    if (!m_pFirstDrawItem || m_bNeedCalcFirstLastDrawItem)
    {
        CalcFirstLastDrawItem();
        m_bNeedCalcFirstLastDrawItem= false;
    }
    ListItemBase* p = m_pFirstDrawItem;
    while (p)
    {
        // TODO: �Ƿ��б�Ҫ����float item 

        CRect rcParent;
        p->GetParentRect(&rcParent);

        if (PtInRect(&rcParent, pt))
        {
            return (LRESULT)p->GetIListItemBase();
        }
        if (p == m_pLastDrawItem)
            break;

        p = p->GetNextVisibleItem();
    }

    return NULL;
}
void  ListCtrlBase::SetFocusRender(IRenderBase* p)
{
    SAFE_RELEASE(m_pFocusRender);
    m_pFocusRender = p;

    if (m_pFocusRender)
        m_pFocusRender->AddRef();
}


bool  ListCtrlBase::InsertItem(ListItemBase* pNewItem, 
                               IListItemBase* pParent, 
                               IListItemBase* pAfter,
                               int nInsertFlags)
{
    if (false == _InsertItemToTree(pNewItem, pParent, pAfter))
        return false;

    pNewItem->SetIListCtrlBase(m_pIListCtrlBase);
    UISendMessage(pNewItem->GetIListItemBase(), UI_WM_INITIALIZE);

    if (nInsertFlags & LF_UPDATEITEMINDEX)
        UpdateItemIndex(pNewItem);

    if (nInsertFlags & LF_LAYOUT)
        LayoutItem(pNewItem, false);

    if (nInsertFlags & LF_REDRAW)
        m_pIListCtrlBase->UpdateObject();

    UIMSG  msg;
    msg.pMsgTo = m_pIListCtrlBase;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_LCN_ITEMADD;
    msg.wParam = (WPARAM)pNewItem->GetIListItemBase();
    msg.lParam = nInsertFlags;
    UISendMessage(&msg);

    return true;
}

bool ListCtrlBase::_InsertRoot(ListItemBase* pNewItem)
{
    if (NULL == m_pFirstItem)
    {
        m_pFirstItem = pNewItem;
        m_pLastItem = pNewItem;
    }
    else
    {
        pNewItem->SetNextItem(m_pFirstItem);
        m_pFirstItem->SetPrevItem(pNewItem);
        m_pFirstItem = pNewItem;
    }
    return true;
}

bool ListCtrlBase::_InsertAfter(ListItemBase* pNewItem, ListItemBase* pInsertAfter)
{
    if (NULL == pInsertAfter)
        return false;

    ListItemBase* pOldNext = pInsertAfter->GetNextItem();
    pInsertAfter->SetNextItem(pNewItem);
    pNewItem->SetPrevItem(pInsertAfter);
    pNewItem->SetParentItem(pInsertAfter->GetParentItem());

    if (NULL != pOldNext)
    {
        pOldNext->SetPrevItem(pNewItem);
        pNewItem->SetNextItem(pOldNext);
    }
    if (m_pLastItem == pInsertAfter)
    {
        m_pLastItem = pNewItem;
    }

    return true;
}

bool ListCtrlBase::_InsertBefore(ListItemBase* pNewItem, ListItemBase* pInsertBefore)
{
    if (NULL == pInsertBefore)
        return false;

    ListItemBase* pOldPrev = pInsertBefore->GetPrevItem();
    pInsertBefore->SetPrevItem(pNewItem);
    pNewItem->SetNextItem(pInsertBefore);
    pNewItem->SetParentItem(pInsertBefore->GetParentItem());

    if (NULL != pOldPrev)
    {
        pOldPrev->SetNextItem(pNewItem);
        pNewItem->SetPrevItem(pOldPrev);
    }
    if (m_pLastItem == pInsertBefore)
    {
        m_pLastItem = pNewItem;
    }
    if (m_pFirstItem == pInsertBefore)
    {
        m_pFirstItem = pNewItem;
    }

    return true;
}

bool ListCtrlBase::_InsertFirstChild(ListItemBase* pNewItem, ListItemBase* pParent)
{
    if (NULL == pParent)
        return false;

	pParent->AddChildFront(pNewItem);
    return true;
}

bool ListCtrlBase::_InsertLastChild(ListItemBase* pNewItem, ListItemBase* pParent)
{
    if (NULL == pParent)
        return false;

    if (NULL == pParent->GetChildItem())
        return this->_InsertFirstChild(pNewItem, pParent);

    pParent->AddChild(pNewItem);
    return true;
}


void ListCtrlBase::ToggleItemExpand(ListItemBase* pItem, bool bUpdate)
{
    if (NULL == pItem)
        return;

    if (pItem->IsExpand())
        CollapseItem(pItem, bUpdate);
    else
        ExpandItem(pItem, bUpdate);
}

void ListCtrlBase::CollapseItem(ListItemBase* pItem, bool bUpdate)
{
    if (NULL == pItem)
        return ;

    if (pItem->IsCollapsed())
        return;

    pItem->SetExpand(false);

    if (NULL == pItem->GetChildItem())
        return;

    // �жϵ�ǰѡ�����Ƿ����أ�������Լ�����ѡ��һЩ���ⳡ����ֻ�����ӽ�㱻ѡ�С������ֲ��䣩
    if (pItem->IsSelectable() && m_pFirstSelectedItem)
    {
        if (pItem->IsMyChildItem(GetFirstSelectItem(), true))
        {
            SelectItem(pItem, false);  // ��ѡ����ת�Ƶ������
        }
    }
    ListItemBase* pFocusItem = GetFocusItem();
    if (pFocusItem)
    {
        if (pItem->IsMyChildItem(pFocusItem, true))
        {
            SetFocusItem(pItem);
        }
    }

    this->SetCalcFirstLastDrawItemFlag();
    this->SetInvalidateAllItems();  // PS: ��Щ�����۵��������ˣ��ǲ����ٵ���RedrawItem�ģ����Ҳ����ȫ��ˢ��
    this->UpdateItemIndex(NULL);
    this->LayoutItem(m_pFirstItem, bUpdate);
}
void ListCtrlBase::ExpandItem(ListItemBase* pItem, bool bUpdate)
{
    if (NULL == pItem)
        return ;

    bool bDoExpandAction = false;
    ListItemBase* pParent = pItem;
    while (pParent)
    {
        if (pParent->IsExpand())
        {
            pParent = pParent->GetParentItem();    
            continue;
        }
         
        bDoExpandAction = true;
        pParent->SetExpand(true);
        pParent = pParent->GetParentItem();    
    }
    
    if (bDoExpandAction)
    {
        this->SetCalcFirstLastDrawItemFlag();
        this->SetInvalidateAllItems();
        this->UpdateItemIndex(NULL);
        this->LayoutItem(m_pFirstItem, bUpdate);
    }
}

void  ListCtrlBase::CollapseAll(bool bUpdate)
{
    ListItemBase*  pItem = m_pFirstItem;
    while (pItem)
    {
        pItem->SetExpand(false, true);
        pItem = pItem->GetNextTreeItem();
    }

    if (bUpdate)
    {
        this->LayoutItem(NULL, true);
    }
}
void  ListCtrlBase::ExpandAll(bool bUpdate)
{
    ListItemBase*  pItem = m_pFirstItem;
    while (pItem)
    {
        pItem->SetExpand(true, true);
        pItem = pItem->GetNextTreeItem();
    }

    if (bUpdate)
    {
        this->LayoutItem(NULL, true);
    }
}

LRESULT  ListCtrlBase::OnGetMouseKeyboardMgr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (NULL == m_pMKMgr)
        return NULL;

    return (LRESULT)static_cast<IMKMgr*>(m_pMKMgr);
}

BOOL  ListCtrlBase::ProcessItemMKMessage(UIMSG* pMsg, ListItemBase* pItem)
{
    if (NULL == m_pMKMgr)
        return FALSE;

    return m_pMKMgr->ProcessItemMessage(pMsg, pItem);
}


// ��p����������
void  ListCtrlBase::RemoveItemFromTree(ListItemBase*  pItem)
{
    if (!pItem)
        return;

    m_nItemCount--;

    // ���б���ɾ��
    // ע�������ȴ��б���ɾ���������selection��
    //     ����fire selection changed��Ӧ�л����õ���ITEM�����±���
    if (pItem->GetPrevItem())
    {
        pItem->GetPrevItem()->SetNextItem(pItem->GetNextItem());
    }
    if (pItem->GetNextItem())
    {
        pItem->GetNextItem()->SetPrevItem(pItem->GetPrevItem());
    }

    // �Լ���Ϊ�����ĵ�һ���ӽ��
    ListItemBase* pParent = pItem->GetParentItem();
    if (pParent)
    {
        if (pParent->GetChildItem() == pItem)
            pParent->SetChildItem(pItem->GetNextItem());

        if (pParent->GetLastChildItem() == pItem)
            pParent->SetLastChildItem(pItem->GetPrevItem());
    }

    if (m_pFirstItem == pItem)
    {
        m_pFirstItem = pItem->GetNextItem();
    }
    if (m_pLastItem == pItem)
    {
        m_pLastItem = pItem->GetPrevItem();
    }

    this->SetCalcFirstLastDrawItemFlag();
    if (pItem == m_pFirstDrawItem || pItem == m_pLastDrawItem)
    {
        m_pFirstDrawItem = NULL;
        m_pLastDrawItem = NULL;
    }

    // ����Ч�б����Ƴ�
    RemoveInvavlidateItem(pItem);

    pItem->SetNextItem(NULL);
    pItem->SetPrevItem(NULL);
    pItem->SetParentItem(NULL);
}

bool  ListCtrlBase::is_my_item(IListItemBase* pItem)
{
    ListItemBase* pValidItem = get_valid_list_item(pItem);
    if (pValidItem == NULL) 
        return true;

    if (pValidItem->GetListCtrlBase() != this)
        return false;

    return true;
}

ListItemBase*  ListCtrlBase::get_valid_list_item(IListItemBase* pItem)
{
    if (pItem == NULL || 
        pItem == UITVI_ROOT ||
        pItem == UITVI_LAST || 
        pItem == UITVI_FIRST)
        return NULL;

    return pItem->GetImpl();
}

bool  ListCtrlBase::MoveItem(ListItemBase* pItem, IListItemBase* pNewParent, IListItemBase* pInsertAfter, long lFlags)
{
    if (!pItem)
        return false;

    // �쳣
    // 1. item����listctrl���б���
    if (pItem->GetListCtrlBase() != this)
        return false;

    if (!is_my_item(pNewParent) || !is_my_item(pInsertAfter))
        return false;

    // 2. parent ���Լ������Լ���������
    if (pItem->GetIListItemBase() == pNewParent || pItem->GetIListItemBase() == pInsertAfter)
        return false;

    ListItemBase* pValidNewParent = get_valid_list_item(pNewParent);
    if (pValidNewParent && pItem->IsMyChildItem(pValidNewParent, true))
        return false;

    ListItemBase* pValidInsertAfter = get_valid_list_item(pInsertAfter);
    if (pValidInsertAfter && pItem->IsMyChildItem(pValidInsertAfter, true))
        return false;

    // �뵱ǰλ��һ�£�����Ҫ�ƶ�
    IListItemBase* pOldParent = NULL;
    IListItemBase* pOldPrev = NULL;
    if (pItem->GetParentItem())
        pOldParent = pItem->GetParentItem()->GetIListItemBase();
    if (pItem->GetPrevItem())
        pOldPrev = pItem->GetPrevItem()->GetIListItemBase();

    if (pOldParent == pNewParent && pOldPrev == pInsertAfter)
        return true;

    // ����״̬Ӱ��
    if (pItem == m_pFirstDrawItem || pItem == m_pLastDrawItem)
    {
        SetCalcFirstLastDrawItemFlag();
    }

    RemoveItemFromTree(pItem);
    _InsertItemToTree(pItem, pNewParent, pInsertAfter);

    if (lFlags & LF_UPDATEITEMINDEX)
    {
        this->UpdateItemIndex(pItem);
    }
    if (lFlags & LF_LAYOUT)
    {
        this->LayoutItem(pItem, false);
    }
    if (lFlags & LF_REDRAW)
    {
        m_pIListCtrlBase->UpdateObject();
    }

    return true;
}

bool  ListCtrlBase::_InsertItemToTree(ListItemBase*  pNewItem, IListItemBase* pParent, IListItemBase* pAfter)
{
    if (NULL == pParent)
        pParent = UITVI_ROOT;
    if (NULL == pAfter)
        pAfter = UITVI_LAST;
    if (UITVI_ROOT == pAfter)
    {
        pParent = UITVI_ROOT;
        pAfter = UITVI_FIRST;
    }

    // ����pParent��ȡֵ: ROOT | xxx
    //      hAfter��ȡֵ : FIRST | LAST | xxx
    bool bRet = false;
    do
    {
        if (UITVI_ROOT == pParent)
        {
            if (UITVI_FIRST == pAfter)
            {
                bRet = _InsertRoot(pNewItem);
            }
            else if (UITVI_LAST == pAfter)
            {
                ListItemBase* pLastRoot = GetLastItem();
                if (NULL == pLastRoot)
                    bRet = _InsertRoot(pNewItem);
                else
                    bRet = _InsertAfter(pNewItem, pLastRoot);
            }
            else
            {
                if (pAfter->GetParentItem() != NULL)  // ���Ǹ����
                    break;

                bRet = _InsertAfter(pNewItem, pAfter?pAfter->GetImpl():NULL);
            }
        }
        else
        {
            if (UITVI_FIRST == pAfter)
            {
                bRet = _InsertFirstChild(pNewItem, pParent?pParent->GetImpl():NULL);
            }
            else if (UITVI_LAST == pAfter)
            {
                bRet = _InsertLastChild(pNewItem, pParent?pParent->GetImpl():NULL);
            }
            else
            {
                if (pAfter->GetParentItem() != pParent)  // ���Ƕ�Ӧ�ӽ��
                    break;

                bRet = _InsertAfter(pNewItem, pAfter?pAfter->GetImpl():NULL);
            }
        }
    }
    while(0);

    if (bRet)
    {
        m_nItemCount++;
        if (pNewItem->GetId() != 0)
        {
            m_mapItem[pNewItem->GetId()] = pNewItem->GetIListItemBase();
        }
        SetCalcFirstLastDrawItemFlag();
    }
    return bRet;
}

void  ListCtrlBase::ModifyListCtrlStyle(ListCtrlStyle* add, ListCtrlStyle* remove)
{
    ListCtrlStyle  oldStyle = m_listctrlStyle;

#define  MODIFY(x) \
    if (add && add->x) \
        m_listctrlStyle.x = 1; \
    if (remove && remove->x) \
        m_listctrlStyle.x = 0;

    MODIFY(sort_ascend);
    MODIFY(sort_descend);
    MODIFY(sort_child);
    MODIFY(multiple_sel);
    MODIFY(popuplistbox);
    MODIFY(menu);
    MODIFY(destroying);
    MODIFY(float_group_head);

    if (oldStyle.menu != m_listctrlStyle.menu ||
        oldStyle.multiple_sel != m_listctrlStyle.multiple_sel ||
        oldStyle.popuplistbox != m_listctrlStyle.popuplistbox)
    {
        update_mouse_mgr_type();
    }
}
bool  ListCtrlBase::TestListCtrlStyle(ListCtrlStyle* test)
{
	if (!test)
		return false;

#define  TEST(x) \
    if (test->x && !m_listctrlStyle.x) \
        return false;

    TEST(sort_ascend);
    TEST(sort_descend);
    TEST(sort_child);
    TEST(multiple_sel);
    TEST(popuplistbox);
    TEST(menu);
    TEST(destroying);
    TEST(float_group_head);

	return true;
}


LRESULT  ListCtrlBase::OnGesturePan(UINT, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);

    //::OutputDebugStringA("ScrollBarManager::OnGesturePan");
    int xOffset = GET_X_LPARAM(wParam);
    int yOffset = GET_Y_LPARAM(wParam);

    bool bNeedUpdate = false;
    if (xOffset)
    {
        if (m_mgrScrollBar.SetHScrollPos(m_mgrScrollBar.GetHScrollPos()-xOffset))
            bNeedUpdate = true;
    }
    if (yOffset)
    {
		if (m_mgrScrollBar.SetVScrollPos(m_mgrScrollBar.GetVScrollPos()-yOffset))      
            bNeedUpdate = true;
    }

    if (bNeedUpdate)
    {
		this->SetCalcFirstLastDrawItemFlag();
        this->UpdateObject();
    }
    return 1;
}