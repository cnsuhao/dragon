#include "stdafx.h"
#include "headerlistctrl.h"
#include "headerlistctrllayout.h"
#include "headerlistitem.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

using namespace UI;

HeaderListCtrl::HeaderListCtrl()
{
    m_pBindCtrl = NULL;
    m_pIHeaderListCtrl = NULL;
    m_pSortArrowRender = NULL;
    m_pCursorDragDivider = NULL;
    m_pCursorDragDividerHidden = NULL;

	m_bEnableDragColumn = true;
    m_bDragingDivider = false;

    m_ptLastDragDivider.x = m_ptLastDragDivider.x = 0;
    m_nOldDragingColoumnWidth = 0;
    m_pLastDragItem = NULL;

//    m_nHScrollOffset = 20;
}
HeaderListCtrl::~HeaderListCtrl()
{
    SAFE_RELEASE(m_pSortArrowRender);
    SAFE_RELEASE(m_pCursorDragDivider);
    SAFE_RELEASE(m_pCursorDragDividerHidden);
}

HRESULT  HeaderListCtrl::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(IHeaderListCtrl, IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    // 设置布局类 
    m_pIHeaderListCtrl->SetLayout(HeaderListItemLayout::CreateInstance(false));

	ObjStyle s = {0};
	s.hscroll = 1;
	ObjStyle sRemove = {0};
	s.vscroll = 1;
    m_pIHeaderListCtrl->ModifyObjectStyle(&s, &sRemove);
    m_pIHeaderListCtrl->GetIScrollBarMgr()->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_HIDE_ALWAYS);

// #ifdef _DEBUG
// #else
//     if (p->IsDesignMode())
// #endif
//     {
//         this->AddColumn( _T("First"), 50);
//         this->AddColumn( _T("Second"), 50);
//         this->AddColumn( _T("Third"), 50);
//     }

    return S_OK;
}

void  HeaderListCtrl::SetBindControl(IMessage* p)
{
    m_pBindCtrl = p;
}
IMessage*  HeaderListCtrl::GetBindControl()
{
    return m_pBindCtrl;
}

void  HeaderListCtrl::OnInitialize()
{
	DO_PARENT_PROCESS(IHeaderListCtrl, IListCtrlBase);

	ObjStyle s = {0};
	s.tabstop = 1;
	m_pIHeaderListCtrl->ModifyObjectStyle(0, &s);     // 不抢listview的焦点
}

void  HeaderListCtrl::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    DO_PARENT_PROCESS(IHeaderListCtrl, IListCtrlBase);

    IUIApplication*  pUIApp = m_pIHeaderListCtrl->GetUIApplication();
    ITextRenderBase*  pTextRender = NULL;
    pMapAttrib->GetAttr_TextRenderBase(NULL, XML_TEXTRENDER_TYPE, true, pUIApp, static_cast<IObject*>(m_pIHeaderListCtrl), &pTextRender);
    if (pTextRender)
    {
        m_pIHeaderListCtrl->SetTextRender(pTextRender);
        pTextRender->Release();
    }
    if (NULL == m_pIHeaderListCtrl->GetTextRender())
    {
        pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_SIMPLE, m_pIHeaderListCtrl, &pTextRender);
        if (pTextRender)
        {
            ISimpleTextRender* p = (ISimpleTextRender*)UISendMessage(pTextRender, UI_WM_QUERYINTERFACE, (WPARAM)&uiiidof(ISimpleTextRender));
            if (p)
            {
                Color c(76,96,122,255);
                p->SetColor(&c);
            }

            SERIALIZEDATA data = {0};
            data.pUIApplication = pUIApp;
            data.pMapAttrib = pMapAttrib;
            data.szPrefix = NULL;
            data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
            pTextRender->Serialize(&data);

            m_pIHeaderListCtrl->SetTextRender(pTextRender);
            pTextRender->Release();
        }
    }

    if (NULL == m_pIHeaderListCtrl->GetForeRender())
    {
        IRenderBase* pRenderBase = NULL;
        pUIApp->CreateRenderBase(HEADER_TYPE_THEME_HEADERCTRL_ITEM_BKGND, m_pIHeaderListCtrl, &pRenderBase);
        if (pRenderBase)
        {
            SERIALIZEDATA data = {0};
            data.pUIApplication = pUIApp;
            data.pMapAttrib = pMapAttrib;
            data.szPrefix = XML_BACKGND_RENDER_PREFIX;
            data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
            pRenderBase->Serialize(&data);

            m_pIHeaderListCtrl->SetForegndRender(pRenderBase);
            SAFE_RELEASE(pRenderBase);
        }
    }

    ICursorRes* pCursorRes = pUIApp->GetActiveSkinCursorRes();
    if (pCursorRes)
    {
        if (NULL == m_pCursorDragDivider)
        {
            pCursorRes->GetCursor(XML_CURSOR_IDC_DRAG_SEPERATOR, &m_pCursorDragDivider);
        }
        if (NULL == m_pCursorDragDividerHidden)
        {
            pCursorRes->GetCursor(XML_CURSOR_IDC_SPLITBAR_V, &m_pCursorDragDividerHidden);
        }
    }
}

void HeaderListCtrl::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    if (NULL == m_pIHeaderListCtrl->GetBkRender())
    {
        CRect rcClient;
        m_pIHeaderListCtrl->GetObjectClientRect(&rcClient);

        CRect rcLastItem(0,0,0,0);
        if (m_pIHeaderListCtrl->GetLastItem())
        {
            m_pIHeaderListCtrl->GetLastItem()->GetParentRect(&rcLastItem);
        }

        // 右侧空白 
        if (rcLastItem.right < rcClient.right)
        {
            CRect rcBlank(rcClient);
            rcBlank.left = rcLastItem.right;
            rcBlank.right += HEADER_DIVIDER_EXTENT_SIZE; // 将分隔条不显示出来

            if (m_pIHeaderListCtrl->GetForeRender())
            {
                m_pIHeaderListCtrl->GetForeRender()->DrawState(pRenderTarget, &rcBlank, 0); 
            }
        }
    }
    else
    {
        SetMsgHandled(FALSE);
    }
}

void  HeaderListCtrl::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = GetColumnTotalWidth();
    pSize->cy = 24;

    IRenderBase* pBkgndRender = m_pIHeaderListCtrl->GetBkRender();
    if (pBkgndRender)
    {
        int nHeight = pBkgndRender->GetDesiredSize().cy;
        if (0 != nHeight)
            pSize->cy = nHeight;
    }
}
// void  HeaderListCtrl::OnGetScrollOffset(int* pxOffset, int* pyOffset)
// {
//     *pxOffset = m_nHScrollOffset;
// }

IHeaderListItem* HeaderListCtrl::AddColumn(LPCTSTR szText, int nWidth, int nFormat)
{
    IHeaderListItem* pItem = NULL;
    IHeaderListItem::CreateInstance(m_pIHeaderListCtrl->GetUIApplication(), &pItem);

    if (szText)
        pItem->SetText(szText);

    pItem->SetConfigWidth(nWidth);
    pItem->SetTextAlign(nFormat);

    // 强制LF_UPDATEITEMINDEX，用于listviewitem获取当前绘制的subitem索引
    if (false == m_pIHeaderListCtrl->AddItem(pItem, LF_UPDATEITEMINDEX))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

	pItem->SetSelectable(false);  // 不需要选中。要不然还会触发makeitemvisible，导致列表移动
    return pItem;
}

IHeaderListItem* HeaderListCtrl::InsertColumn(int nPos, LPCTSTR szText, int nWidth, int nFormat)
{
    IHeaderListItem* pItem = NULL;
    IHeaderListItem::CreateInstance(m_pIHeaderListCtrl->GetUIApplication(), &pItem);

    if (szText)
        pItem->SetText(szText);

    pItem->SetConfigWidth(nWidth);
    pItem->SetTextAlign(nFormat);

    // 强制LF_UPDATEITEMINDEX，用于listviewitem获取当前绘制的subitem索引
    if (false == m_pIHeaderListCtrl->InsertItem(pItem, nPos, LF_UPDATEITEMINDEX)) 
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

	pItem->SetSelectable(false);  // 不需要选中。要不然还会触发makeitemvisible，导致列表移动
    return pItem;
}

int   HeaderListCtrl::GetColumnCount()
{
    return m_pIHeaderListCtrl->GetItemCount();
}

bool HeaderListCtrl::CanDragColumn()
{
    return m_bEnableDragColumn;
}

void  HeaderListCtrl::SetHScrollOffset(int nhScrollOffset)
{
    m_pIHeaderListCtrl->GetIScrollBarMgr()->SetHScrollPos(nhScrollOffset);
}
int   HeaderListCtrl::GetHScrollOffset()
{
    return m_pIHeaderListCtrl->GetIScrollBarMgr()->GetHScrollPos();
}


//
// 备注：
//   1. 如果允许拖拽时，当鼠标位于位于下一列的前5px位置时，也返回鼠标在上一列
//   2. 在检测时，应尽量检测最后一个符合条件的列，如一列被拖拽到隐藏的情况下
//   
IListItemBase* HeaderListCtrl::HitTest(POINT ptWindow, HEADERLISTCTRL_HITTEST* peHitTest)
{
    IListItemBase* pRetItem = NULL;

    if (peHitTest)
        *peHitTest = HDC_HT_ITEM_NOWHERE;

    CRect rcClient;
    m_pIHeaderListCtrl->GetClientRectInObject(&rcClient);

    // 转换为内部坐标

    POINT pt;
    m_pIHeaderListCtrl->WindowPoint2ObjectPoint(&ptWindow, &pt, true);
    if (FALSE == rcClient.PtInRect(pt))
        return NULL;

    m_pIHeaderListCtrl->ObjectPoint2ObjectClientPoint(&pt, &pt);
    REGION4 rcNonClient;
    m_pIHeaderListCtrl->GetNonClientRegion(&rcNonClient);

    int nDividerWidth = HEADER_DIVIDER_EXTENT_SIZE;
    int x1 = rcNonClient.left;
    int x2 = x1;

    IListItemBase*  pItem = m_pIHeaderListCtrl->FindVisibleItemFrom(NULL);
    while (pItem)
    {
        CRect rcItem;
        pItem->GetParentRect(&rcItem);
        x1 = rcItem.left;
        x2 = rcItem.right;

        if (pRetItem)
        {
            if (x1 == x2)  // 该列宽度为0，是被隐藏的一列
            {
                if (peHitTest)
                    *peHitTest = HDC_HT_DIVIDER_HIDDEN_ITEM;

                pRetItem = pItem;
                pItem = pItem->GetNextVisibleItem();
                continue;
            }
            else
            {
                break;
            }
        }

        if (false == CanDragColumn())
        {
            if (pt.x >= x1 && pt.x <= x2)
            {
                pRetItem = pItem;

                if (peHitTest)
                    *peHitTest = HDC_HT_ITEM;

                break;
            }
        }
        else
        {
            if (pt.x >= x1 && pt.x < (x2+nDividerWidth))  // 不用 <= ，会造成鼠标样式与hover不一致
            {
                pRetItem = pItem;
                if (peHitTest)
                    *peHitTest = HDC_HT_ITEM;

                if (pt.x >= x2-nDividerWidth)    // 判断鼠标是否位于拖拽处
                {
                    IObject* pHover = m_pIHeaderListCtrl->GetHoverObject();
                    if (!pHover)
                        pHover = m_pIHeaderListCtrl->GetPressObject();

                    if (!pHover || pHover->IsRejectMouseMsgAll())
                    {
                        if (peHitTest)
                            *peHitTest = HDC_HT_DIVIDER;
                    } 

                    if (pt.x < x2)  // 如果存在隐藏项，那么分隔条以左，用于拖拽，分隔条以右用于显示隐藏项
                    {
                        break;
                    }
                    else
                    {
                        pItem = pItem->GetNextVisibleItem();
                        continue;      // 继续检测该分隔条后面有没有隐藏项. 当鼠标位于分隔条以左，不去检测隐藏项
                    }
                }
                break;
            }
        }
        pItem = pItem->GetNextVisibleItem();
    }

    return pRetItem;
}


BOOL  HeaderListCtrl::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
    if (CanDragColumn())
    {
        IObject* pHover = m_pIHeaderListCtrl->GetHoverObject();
        if (!pHover)
            pHover = m_pIHeaderListCtrl->GetPressObject();

        if (pHover && !pHover->IsRejectMouseMsgAll())
        {
            SetMsgHandled(FALSE);
            return FALSE;
        }

        POINT pt;
        GetCursorPos(&pt);
        ::MapWindowPoints(NULL, m_pIHeaderListCtrl->GetHWND(), &pt, 1);

        HEADERLISTCTRL_HITTEST eHitTest = HDC_HT_ITEM_NOWHERE;
        HitTest(pt, &eHitTest);

        if (eHitTest == HDC_HT_DIVIDER_HIDDEN_ITEM && NULL != m_pCursorDragDividerHidden)
        {
            ::SetCursor(m_pCursorDragDividerHidden->GetCursor());
        }
        else if (eHitTest == HDC_HT_DIVIDER && NULL != m_pCursorDragDivider)
        {
            ::SetCursor(m_pCursorDragDivider->GetCursor());
        }
        else
        {
            SetMsgHandled(FALSE);
        }
    }
    else
    {
        SetMsgHandled(FALSE);
    }

    return TRUE;
}

void HeaderListCtrl::OnMouseMove(UINT nFlags, POINT point)
{
    if (m_bDragingDivider)
    {
        UIASSERT(NULL != m_pLastDragItem);
        CRect rcItem;
        m_pLastDragItem->GetParentRect(&rcItem);

        int nSaveWidth = rcItem.Width();
        int nNewWidth = m_nOldDragingColoumnWidth - (m_ptLastDragDivider.x - point.x);
        if (nNewWidth < 0)
            nNewWidth = 0;

        if (nNewWidth != nSaveWidth)
        {
            m_pLastDragItem->SetConfigWidth(nNewWidth);
            m_pIHeaderListCtrl->LayoutItem(m_pLastDragItem, false);

            if (NULL == m_pIHeaderListCtrl->GetNotifyObj())
            {
                m_pIHeaderListCtrl->UpdateObject();
            }
            else
            {
                UIMSG  msg;
                msg.pMsgFrom = m_pIHeaderListCtrl;
                msg.message = UI_WM_NOTIFY;
                msg.nCode = UI_HDN_TRACKING;
                msg.wParam = GetColumnItemIndex(m_pLastDragItem);
                msg.lParam = (LPARAM)&(nNewWidth);

                if (m_pIHeaderListCtrl->DoNotify(&msg))
                {
                    FireTotalWidthChanged();
                }
                else  // 没处理
                {
                    m_pIHeaderListCtrl->UpdateObject();
                }
            }
        }
        return;
    }

    SetMsgHandled(FALSE);
}

void HeaderListCtrl::OnLButtonDown(UINT nFlags, POINT point)
{
    HEADERLISTCTRL_HITTEST eHitTest = HDC_HT_ITEM_NOWHERE;
    IListItemBase* pHoverItem = this->HitTest(point, &eHitTest);
//    point.x += m_nHScrollOffset;

    if ((eHitTest == HDC_HT_DIVIDER||eHitTest==HDC_HT_DIVIDER_HIDDEN_ITEM) && pHoverItem)
    {
        UIMSG  msg;
        msg.pMsgFrom = m_pIHeaderListCtrl;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_HDN_BEGIN_TRACK;
        msg.wParam = GetColumnItemIndex(pHoverItem);

        long lRet = m_pIHeaderListCtrl->DoNotify(&msg);
        if (0 == lRet)  // 返回非0取消拖拽
        {
            m_ptLastDragDivider = point;

            CRect rcItem;
            pHoverItem->GetParentRect(&rcItem);
            m_nOldDragingColoumnWidth = rcItem.Width();
            m_bDragingDivider = true;
            m_pLastDragItem = pHoverItem;
        }
    }
    else
    {
        SetMsgHandled(FALSE);
    }
}
void HeaderListCtrl::OnLButtonUp(UINT nFlags, POINT point)
{
    if (m_bDragingDivider)
    {
        OnCancelMode();
    }

    SetMsgHandled(FALSE);
}

void HeaderListCtrl::OnCancelMode()
{
    if (m_bDragingDivider)
    {

        UIMSG  msg;
        msg.pMsgFrom = m_pIHeaderListCtrl;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_HDN_END_TRACK;
        if (m_pLastDragItem)
            msg.wParam = GetColumnItemIndex(m_pLastDragItem);

        m_pIHeaderListCtrl->DoNotify(&msg);

        m_bDragingDivider = false;
        m_ptLastDragDivider.x = m_ptLastDragDivider.x = 0;
        m_nOldDragingColoumnWidth = 0;
        m_pLastDragItem = NULL;
    }
}

int HeaderListCtrl::GetColumnItemIndex(IListItemBase* pTestItem)
{
    if (NULL == pTestItem)
        return -1;

    IListItemBase*  pItem = m_pIHeaderListCtrl->GetFirstItem();
    int i = 0;
    while (pItem)
    {
        if (pItem == pTestItem)
            return i;

        i++;
        pItem = pItem->GetNextItem();
    }

    return -1;
}

// 通知外部header ctrl的宽度发生改变
void  HeaderListCtrl::FireTotalWidthChanged()
{
//    m_pColumnsInfo.UpdateWidth();

    UIMSG  msg;
    msg.pMsgFrom = m_pIHeaderListCtrl;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_HDN_TOTAL_WIDTH_CHANGED;
    msg.wParam = GetColumnTotalWidth();
    m_pIHeaderListCtrl->DoNotify(&msg);
}

int  HeaderListCtrl::GetColumnTotalWidth()
{
    int nRet = 0;
    IListItemBase* pItem = m_pIHeaderListCtrl->FindVisibleItemFrom(NULL);
    CRect  rcItem;
    while (pItem)
    {
        pItem->GetParentRect(&rcItem);

        nRet += rcItem.Width();
        nRet += m_pIHeaderListCtrl->GetHorzSpacing();
        pItem = pItem->GetNextVisibleItem();
    }

    return nRet;
}

// ColumnsInfoImpl*  HeaderListCtrl::GetColumnsInfo() 
// {
//     return &m_pColumnsInfo; 
// }

LRESULT  HeaderListCtrl::OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT pt = {GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam) };

    IListItemBase* pRet = NULL;

    IListItemBase* p = m_pIHeaderListCtrl->GetFirstDrawItem();
    CRect rcParent;
    while (p)
    {
        p->GetParentRect(&rcParent);
        rcParent.right += HEADER_DIVIDER_EXTENT_SIZE;

        if (PtInRect(&rcParent, pt))
        {
            pRet = p;

            // 如果不是位于分隔条上面，直接返回。否则需要判断是否是位于一个隐藏ITEM上
            if (pt.x < rcParent.right - HEADER_DIVIDER_EXTENT_SIZE)
                return (LRESULT)pRet;

            break;
        }

        if (p == m_pIHeaderListCtrl->GetLastDrawItem())
            break;

        p = p->GetNextVisibleItem();
    }

    if (NULL == pRet)
        return NULL;

    // 判断下一下是否是隐藏item
    p = pRet;
    while (p)
    {
        p->GetParentRect(&rcParent);
        if (rcParent.left == rcParent.right)
        {
            pRet = p;
        }
        p = p->GetNextVisibleItem();
    }
    return (LRESULT)pRet;
}


void HeaderListCtrl::OnCalcParentNonClientRect(CRegion4* prcNonClientRegion)
{
    if (false == m_pIHeaderListCtrl->IsSelfVisible())
        return;

    int nHeight = m_pIHeaderListCtrl->GetHeight();
    if (0 == nHeight)
    {
        SIZE s = {0, 0};
        this->GetDesiredSize(&s);
        nHeight = s.cy;
    }
    prcNonClientRegion->top += nHeight;
}


LRESULT  HeaderListCtrl::OnAddItem(WPARAM w, LPARAM l)
{
    SetMsgHandled(FALSE);
    if (l & LF_LAYOUT)
        m_pIHeaderListCtrl->LayoutItem((IListItemBase*)w, false);

    UIMSG msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_HDN_ADDCOLUMN;
    msg.wParam = (WPARAM)(IListItemBase*)w;
    m_pIHeaderListCtrl->DoNotify(&msg);

    FireTotalWidthChanged();
    return 0;
}
LRESULT  HeaderListCtrl::OnPostRemoveItem(WPARAM w, LPARAM l)
{
    SetMsgHandled(FALSE);

    IListItemBase* pItem = (IListItemBase*)l;

    UIMSG msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_HDN_REMOVECOLUMN;
    msg.wParam = (WPARAM)pItem;
    m_pIHeaderListCtrl->DoNotify(&msg);

    FireTotalWidthChanged();
    return 0;
}
LRESULT  HeaderListCtrl::OnPostRemoveAllItem(WPARAM w, LPARAM l)
{
    SetMsgHandled(FALSE);

    UIMSG msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_HDN_CLEARCOLUMN;
    msg.wParam = (WPARAM)(IListItemBase*)w;
    m_pIHeaderListCtrl->DoNotify(&msg);

    FireTotalWidthChanged();
    return 0;
}
