#include "stdafx.h"
#include "d2drenderbitmap.h"

namespace UI
{

SIZE  Direct2DIconRenderBitmap::GetDrawSize()
{
    SIZE s = { m_nIconWidth, m_nIconHeight };
    return s;
}
void  Direct2DIconRenderBitmap::SetDrawSize(SIZE* ps)
{
    if (!ps)
        return;

    ps->cx = m_nIconWidth;
    ps->cy = m_nIconHeight;
}




Direct2DImageListRenderBitmap::Direct2DImageListRenderBitmap()
{
    m_nCount = 0;
    m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
}

int Direct2DImageListRenderBitmap::GetItemWidth()
{
    if (0 == m_nCount)
        return 0;

    switch(m_eLayout)
    {
    case IMAGELIST_LAYOUT_TYPE_V:
        return GetWidth();

    case IMAGELIST_LAYOUT_TYPE_H:
        return GetWidth()/m_nCount;
    }

    return 0;
}
int Direct2DImageListRenderBitmap::GetItemHeight()
{
    if (0 == m_nCount)
        return 0;

    switch(m_eLayout)
    {
    case IMAGELIST_LAYOUT_TYPE_H:
        return GetHeight();

    case IMAGELIST_LAYOUT_TYPE_V:
        return GetHeight()/m_nCount;
    }

    return 0;
}
void  Direct2DImageListRenderBitmap::SetItemCount(int n)
{
    m_nCount = n;
}
void  Direct2DImageListRenderBitmap::SetLayoutType(IMAGELIST_LAYOUT_TYPE e)
{
    m_eLayout = e;
}
IMAGELIST_LAYOUT_TYPE Direct2DImageListRenderBitmap::GetLayoutType()
{
    return m_eLayout;
}
bool Direct2DImageListRenderBitmap::GetIndexPos(int nIndex, POINT* pPoint)
{
    if (NULL == pPoint)
        return false;

    pPoint->x = pPoint->y = 0;
    if (nIndex > m_nCount)
        return false;

    if (IMAGELIST_LAYOUT_TYPE_H == m_eLayout)
    {
        pPoint->x = nIndex*GetItemWidth();
        pPoint->y = 0;
    }
    else if (IMAGELIST_LAYOUT_TYPE_V == m_eLayout)
    {
        pPoint->x = 0;
        pPoint->y = nIndex*GetItemHeight();
    }
    else 
        return false;

    return true;
}
}