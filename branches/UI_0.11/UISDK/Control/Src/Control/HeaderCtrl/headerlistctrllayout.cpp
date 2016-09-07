#include "stdafx.h"
#include "headerlistctrllayout.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistitem.h"

using namespace UI;

void  HeaderListItemLayout::Release()
{
    delete this;
}

void HeaderListItemLayout::Arrange(
    IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    if (m_bPercentMode)
    {
        ArrangeByPercent(pStartToArrange, pSizeContent);
        return;
    }

    CRect  rcClient;
    CRect  rcItem;
    m_pIListCtrlBase->GetClientRectInObject(&rcClient);

    int  nHeight = rcClient.Height();
    int  xLeft = 0;
    int  nTotalWidth = 0;
    int  yPrevItemRight = 0;
    int  nHSpace = m_pIListCtrlBase->GetHorzSpacing();

    IListItemBase* pItem = m_pIListCtrlBase->
        FindVisibleItemFrom(pStartToArrange);
    if (NULL == pItem)
        return;

    IListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (NULL == pPrevItem)   // 第一个
    {
        CRect rc(0, 0, GetItemWidth(pItem), nHeight);
        pItem->SetParentRect(&rc);

        yPrevItemRight = rc.right;
        pItem = pItem->GetNextVisibleItem();
    }
    else
    {
        yPrevItemRight = pPrevItem->GetParentRectPtr()->right;
    }

    while (pItem)
    {
        xLeft = yPrevItemRight + nHSpace;
        yPrevItemRight = xLeft + GetItemWidth(pItem);

        rcItem.SetRect(xLeft, 0, yPrevItemRight, nHeight);
        pItem->SetParentRect(&rcItem);
        pItem = pItem->GetNextVisibleItem();
    }

    if (pSizeContent)
    {
        pSizeContent->cx = yPrevItemRight;
        pSizeContent->cy = nHeight;
    }
}

void  HeaderListItemLayout::Measure(SIZE* pSize)
{
    if (m_bPercentMode)
    {
        pSize->cx = m_pIListCtrlBase->GetWidth();
        pSize->cy = m_pIListCtrlBase->GetItemHeight();
        return;
    }

    pSize->cx = pSize->cy = 0;
    int  nCount = m_pIListCtrlBase->GetItemCount();

    if (0 == nCount)
        return;

    int  nHSpace = m_pIListCtrlBase->GetHorzSpacing();

    UI::IListItemBase*  pItem = m_pIListCtrlBase->FindVisibleItemFrom(NULL);
    while (pItem)
    {
        SIZE s = {0,0};
        pItem->GetDesiredSize(&s);
        pSize->cx += s.cx;
        pSize->cx += nHSpace;

        pItem = pItem->GetNextVisibleItem();
    }
}


int  HeaderListItemLayout::GetItemWidth(IListItemBase*  pItem)
{
    SIZE s = {0,0};
    pItem->GetDesiredSize(&s);

    return s.cx;
}


void  HeaderListItemLayout::ArrangeByPercent(
    IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    CRect  rcClient;
    CRect  rcItem;
    m_pIListCtrlBase->GetClientRectInObject(&rcClient);

    int  nHeight = rcClient.Height();
    int  nRemainWidth = rcClient.Width();
    int  nPercentCount = 0;
    int  xLeft = 0;
    int  yPrevItemRight = 0;
    int  nHSpace = m_pIListCtrlBase->GetHorzSpacing();

    IListItemBase* pItem = m_pIListCtrlBase->FindVisibleItemFrom(NULL);
    if (NULL == pItem)
        return;

    // 先统计非比例布局项占用的宽度
    while (pItem)
    {
        const ColumnInfo* pInfo = (const ColumnInfo*)UISendMessage(
            pItem, UI_HDM_GETITEM_COLUMNINFO);
        if (!pInfo)
        {
            pItem = pItem->GetNextVisibleItem();
            continue;
        }

        if (COLUMN_WIDTH_PERCENT != pInfo->eWidthType)
        {
            nRemainWidth -= pItem->GetConfigWidth();
        }
        else
        {
            nPercentCount += pItem->GetConfigWidth();
        }
        pItem = pItem->GetNextVisibleItem();
    }
    float fp = 0;
    if (nPercentCount)
        fp = nRemainWidth/(float)nPercentCount;

    pItem = m_pIListCtrlBase->FindVisibleItemFrom(NULL);
    while (pItem)
    {
        xLeft = yPrevItemRight + nHSpace;
        const ColumnInfo* pInfo = (const ColumnInfo*)UISendMessage(
            pItem, UI_HDM_GETITEM_COLUMNINFO);
        if (!pInfo)
        {
            pItem = pItem->GetNextVisibleItem();
            continue;
        }

        if (COLUMN_WIDTH_PERCENT == pInfo->eWidthType)
        {
            yPrevItemRight = xLeft + (int)(pItem->GetConfigWidth()*fp);
        }
        else
        {
            yPrevItemRight = xLeft + pItem->GetConfigWidth();
        }

        rcItem.SetRect(xLeft, 0, yPrevItemRight, nHeight);
        pItem->SetParentRect(&rcItem);
        pItem = pItem->GetNextVisibleItem();
    }

    if (pSizeContent)
    {
        pSizeContent->cx = rcClient.Width();
        pSizeContent->cy = rcClient.Height();
    }
}
