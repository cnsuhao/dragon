#include "stdafx.h"
#include "headerlistitem.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistctrl.h"

using namespace UI;

HeaderListItem::HeaderListItem()
{
    m_pIHeaderListItem = NULL;
    m_pHeaderCtrl = NULL;
    
    m_nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;;
    m_nIndentLeft = HEADER_DIVIDER_EXTENT_SIZE;
    m_nIndentRight = HEADER_DIVIDER_EXTENT_SIZE;
    m_eWidthType = COLUMN_WIDTH_NORMAL;
    m_nMaxWidth = NDEF;
    m_nMinWidth = NDEF;
}

HeaderListItem::~HeaderListItem()
{

}

void  HeaderListItem::OnInitialize()
{
    SetMsgHandled(FALSE);

    IHeaderListCtrl* pHeaderCtrl = (IHeaderListCtrl*)
        m_pIHeaderListItem->GetIListCtrlBase()->
            QueryInterface(uiiidof(IHeaderListCtrl));

    if (pHeaderCtrl)
        m_pHeaderCtrl = pHeaderCtrl->GetImpl();
}

void  HeaderListItem::SetIHeaderListItem(IHeaderListItem* p) 
{
    m_pIHeaderListItem = p; 
    SetIMessageProxy(static_cast<IMessage*>(p)); 
}
IHeaderListItem*  HeaderListItem::GetIHeaderListItem()
{
    return m_pIHeaderListItem; 
}

void  HeaderListItem::SetTextAlign(int n)
{
    m_nFormat = n;
}
int   HeaderListItem::GetTextAlign() 
{
    return m_nFormat; 
}

void  HeaderListItem::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    UINT nRenderState = 0;
    CRect  rcItem;
    m_pIHeaderListItem->GetParentRect(&rcItem);
    IRenderBase* pRender = m_pIHeaderListItem->
        GetIListCtrlBase()->GetForeRender();
    if (pRender)
    {
        nRenderState = m_pIHeaderListItem->GetItemRenderState();
        pRender->DrawState(pRenderTarget, &rcItem, nRenderState);
    }

    CRect rcText = rcItem;
    ITextRenderBase* pTextRender = m_pIHeaderListItem->
        GetIListCtrlBase()->GetTextRender();
    if (pTextRender)
    {
        rcText.left += HEADER_DIVIDER_EXTENT_SIZE;
        rcText.right -= HEADER_DIVIDER_EXTENT_SIZE;

        // 产生两像素按下的效果
        if (nRenderState & RENDER_STATE_PRESS)
        {
            rcText.top += 2;
            rcText.left += 2;
        }

        pTextRender->DrawState(pRenderTarget, &rcText, 
            nRenderState, m_pIHeaderListItem->GetText(), 
            m_nFormat);
    }
}

LRESULT  HeaderListItem::OnGetColumnInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ColumnInfo* pInfo = (ColumnInfo*)wParam;
    GetColumnInfo(pInfo);

    return 0;
}

void  HeaderListItem::SetColumnInfo(ColumnInfo* pInfo)
{
    if (!pInfo)
        return;

    if (pInfo->nMask & CIM_ID)
        m_pIHeaderListItem->SetId(pInfo->nColumnId);
    if (pInfo->nMask & CIM_TEXT)
        m_pIHeaderListItem->SetText(pInfo->szText);
    if (pInfo->nMask & CIM_WIDTH)
        m_pIHeaderListItem->SetConfigWidth(pInfo->nWidth);
    if (pInfo->nMask & CIM_WIDTH_TYPE)
        m_eWidthType = pInfo->eWidthType;

    if (pInfo->nMask & CIM_MIN_WIDTH)
        m_nMinWidth = pInfo->nMinWidth;
    if (pInfo->nMask & CIM_MAX_WIDTH)
        m_nMaxWidth = pInfo->nMaxWidth;
    if (pInfo->nMask & CIM_FORMAT)
        m_nFormat = pInfo->nFormat;
    if (pInfo->nMask & CIM_INDENT_LEFT)
        m_nIndentLeft = pInfo->nIndentLeft;
    if (pInfo->nMask & CIM_INDENT_RIGHT)
        m_nIndentRight = pInfo->nIndentRight;
}

void  HeaderListItem::GetColumnInfo(ColumnInfo* pInfo)
{
    if (!pInfo)
        return;

    if (pInfo->nMask & CIM_ORDER)
        pInfo->nOrder = m_pIHeaderListItem->GetLineIndex();
    if (pInfo->nMask & CIM_ID)
        pInfo->nColumnId = (long)m_pIHeaderListItem->GetId();
    if (pInfo->nMask & CIM_TEXT)
        pInfo->szText = m_pIHeaderListItem->GetText();
    if (pInfo->nMask & CIM_WIDTH)
        pInfo->nWidth = m_pIHeaderListItem->GetConfigWidth();
    if (pInfo->nMask & CIM_WIDTH_TYPE)
        pInfo->eWidthType = m_eWidthType;

    if (pInfo->nMask & CIM_MIN_WIDTH)
        pInfo->nMinWidth = m_nMinWidth;
    if (pInfo->nMask & CIM_MAX_WIDTH)
        pInfo->nMaxWidth = m_nMaxWidth;
    if (pInfo->nMask & CIM_FORMAT)
        pInfo->nFormat = m_nFormat;
    if (pInfo->nMask & CIM_INDENT_LEFT)
        pInfo->nIndentLeft = m_nIndentLeft;
    if (pInfo->nMask & CIM_INDENT_RIGHT)
        pInfo->nIndentRight = m_nIndentRight;
}