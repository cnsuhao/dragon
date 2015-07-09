#include "stdafx.h"
#include "listview.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Control\Src\Control\ListView\layout\listviewitemreportlayout.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"

using namespace UI;
#define HEADER_ID TEXT("__header")

ListView::ListView()
{
	m_pHeaderCtrl = NULL;
    m_pIListView = NULL;
}

HRESULT  ListView::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IListView, IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

//	m_pIListView->ModifyStyle(LISTCTRLBASE_STYLE_MULTIPLE_SEL, 0, true);

    m_pIListView->SetLayout(ListViewItemReportLayout::CreateInstance());
	return S_OK;
}

ListView::~ListView()
{
	m_pHeaderCtrl = NULL;
}

void  ListView::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
	DO_PARENT_PROCESS_MAPID(IListView, IListCtrlBase, UIALT_CALLLESS);

	EditorAddObjectResData data = {0};
	data.pUIApp = pData->pUIApp;
	data.pParentXml = pData->pXml;
	data.pParentObj = m_pIListView;

	data.objiid = uiiidof(IHeaderListCtrl);
	data.ppCreateObj = (void**)&m_pHeaderCtrl;
	data.bNcChild = true;
	data.szId = HEADER_ID;
	pData->pEditor->AddObjectRes(&data);

	m_vscrollbarCreator.CreateByEditor(pData, m_pIListView);
	m_hscrollbarCreator.CreateByEditor(pData, m_pIListView);
}

void  ListView::OnInitialize()
{
	DO_PARENT_PROCESS(IListView, IListCtrlBase);

	IObject* pObj = m_pIListView->FindNcChildObject(HEADER_ID);
	if (pObj)
	{
		m_pHeaderCtrl = (IHeaderListCtrl*)pObj->QueryInterface(
			uiiidof(IHeaderListCtrl));
	}
	if (m_pHeaderCtrl)
	{
        m_pHeaderCtrl->SetBindControl(m_pIListView);
		m_pHeaderCtrl->SetNotify(m_pIListView, 0);
	}

	m_vscrollbarCreator.Initialize(m_pIListView, m_pIListView->GetIScrollBarMgr());
	m_hscrollbarCreator.Initialize(m_pIListView, m_pIListView->GetIScrollBarMgr());
}

void  ListView::OnSize(UINT nType, int cx, int cy)
{
	SetMsgHandled(FALSE);
	this->UpdateHeaderCtrlPos();
}

void ListView::UpdateHeaderCtrlPos()
{
	if (m_pHeaderCtrl)
	{
		int nHeaderCtrlHeight = m_pHeaderCtrl->GetHeight();
		if (0 == nHeaderCtrlHeight)
		{
			nHeaderCtrlHeight = m_pHeaderCtrl->GetDesiredSize().cy;
		}

		REGION4 rListViewNonClient;
		REGION4 rListViewBorder;
		REGION4 rHeaderMargin;
		m_pIListView->GetNonClientRegion(&rListViewNonClient);
		m_pIListView->GetBorderRegion(&rListViewBorder);
		m_pHeaderCtrl->GetMarginRegion(&rHeaderMargin);

		m_pHeaderCtrl->SetObjectPos(
			rHeaderMargin.left + rListViewBorder.left,
			rHeaderMargin.top + rListViewBorder.top, 
			m_pIListView->GetWidth() - (rHeaderMargin.left+rHeaderMargin.right+rListViewNonClient.left+rListViewNonClient.right),
            nHeaderCtrlHeight, 
            SWP_NOREDRAW|SWP_FORCESENDSIZEMSG);

        // TODO: 在这里设置一次是为了解决listview第一次显示时，headerctrl没有通知listview totalwidth的问题
        //       关键是headerctrl不知道第一次如何去通知。外部插入一列后，不一定就会updateitemrect
        ListViewItemReportLayout* pReportLayout = static_cast<ListViewItemReportLayout*>(m_pIListView->GetLayout());
        if (pReportLayout)
        {
            pReportLayout->SetHeaderCtrlWidth(m_pHeaderCtrl->GetColumnTotalWidth());
        }
	}
}
void ListView::OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
    IScrollBarManager*  pScrollBarMgr = m_pIListView->GetIScrollBarMgr();
    pScrollBarMgr->ProcessMessage(GetCurMsg());

    if (m_pHeaderCtrl)
    {
        m_pHeaderCtrl->SetHScrollOffset(pScrollBarMgr->GetHScrollPos());
        m_pHeaderCtrl->UpdateObject();
    }
}

// 如果是纵向滚动条不可见，横向滚动条可见的情况下，使用mousewheel则是横向滚动，这个时候需要更新header偏移
BOOL  ListView::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
    IScrollBarManager*  pScrollBarMgr = m_pIListView->GetIScrollBarMgr();
    
    int nPosSave = pScrollBarMgr->GetHScrollPos();
    DO_PARENT_PROCESS(IListView, IListCtrlBase);
    int nPos = pScrollBarMgr->GetHScrollPos();

    if (nPosSave != nPos)
    {
        m_pHeaderCtrl->SetHScrollOffset(nPos);
        m_pHeaderCtrl->UpdateObject();
    }
    return TRUE;
}

BOOL ListView::OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth)
{
	return TRUE;
}

void ListView::OnHeaderCtrlTotalWidthChanged(int nTotalWidth)
{ 
    ListViewItemReportLayout* pReportLayout = static_cast<ListViewItemReportLayout*>(m_pIListView->GetLayout());
	if (pReportLayout)
	{
		pReportLayout->SetHeaderCtrlWidth(nTotalWidth);
	}

	m_pIListView->LayoutAllItemsAndRedraw();
	m_pHeaderCtrl->SetHScrollOffset(m_pIListView->GetIScrollBarMgr()->GetHScrollPos());
	m_pHeaderCtrl->UpdateObject();
}

LRESULT  ListView::OnGetHeaderCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)m_pHeaderCtrl;
}

bool ListView::SetItemText(int nItem, int nSubItem, LPCTSTR szText, bool bUpdate)
{
	if (nItem >= m_pIListView->GetItemCount())
		return false;

	if (nSubItem >= GetColumnCount())
		return false;

	IListItemBase* pItem = m_pIListView->GetItemByPos(nItem);
	if (NULL == pItem)
		return false;

    UISendMessage(pItem, UI_LVIM_SETSUBITEMTEXT, (WPARAM)nSubItem, (LPARAM)szText);

	if (bUpdate)
		m_pIListView->UpdateObject();
	
	return true;
}

int  ListView::GetColumnCount()
{
	if (NULL == m_pHeaderCtrl)
		return 0;
	
	return m_pHeaderCtrl->GetColumnCount();
}	

IListViewItemBase*  ListView::AddTextItem(LPCTSTR szText, int nAddFlag)
{
    IListViewItemBase* pItem = NULL;
    IListViewItemBase::CreateInstance(m_pIListView->GetUIApplication(), &pItem);
	if (szText)
	{
		pItem->SetText(szText);
	}

    m_pIListView->AddItem(pItem, nAddFlag);
    return pItem;
}

IListViewItemBase*  ListView::InsertTextItem(int nIndex, LPCTSTR szText, int nInsertFlag)
{
    IListViewItemBase* pItem = NULL;
    IListViewItemBase::CreateInstance(m_pIListView->GetUIApplication(), &pItem);

	if (szText)
	{
		pItem->SetText(szText);
	}
    m_pIListView->InsertItem(pItem, nIndex, nInsertFlag);
    return pItem;
}

void  ListView::OnAddColumn(IListItemBase* pAddColumn)
{
    long lIndex = pAddColumn->GetLineIndex();
    IListItemBase* pItem = m_pIListView->GetFirstItem();

    UIMSG  msg;
    msg.message = UI_LVIM_ADDCOLUMN;
    msg.wParam = lIndex;
    msg.pMsgFrom = m_pIListView;
    while (pItem)
    {
        msg.pMsgTo = pItem;
        UISendMessage(&msg);
        pItem = pItem->GetNextTreeItem();
    }
}
void  ListView::OnRemoveColumn(IListItemBase* pRemoveColumn)
{
    long lIndex = pRemoveColumn->GetLineIndex();
    IListItemBase* pItem = m_pIListView->GetFirstItem();

    UIMSG  msg;
    msg.message = UI_LVIM_REMOVECOLUMN;
    msg.wParam = lIndex;
    msg.pMsgFrom = m_pIListView;
    while (pItem)
    {
        msg.pMsgTo = pItem;
        UISendMessage(&msg);
        pItem = pItem->GetNextTreeItem();
    }
}
void  ListView::OnClearColumn()
{
    IListItemBase* pItem = m_pIListView->GetFirstItem();

    UIMSG  msg;
    msg.message = UI_LVIM_CLEARCOLUMN;
    msg.pMsgFrom = m_pIListView;
    while (pItem)
    {
        msg.pMsgTo = pItem;
        UISendMessage(&msg);
        pItem = pItem->GetNextTreeItem();
    }
}