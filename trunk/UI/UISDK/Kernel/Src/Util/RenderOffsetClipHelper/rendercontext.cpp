#include "stdafx.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

//  pInitRect
//     一般是取 window对象的 &m_rcParent，如果是只刷新一个控件，则直接指为NULL就行了，在begindraw中会指定该对象的剪裁区域。
//
//  bClip
//     是否需要维护HDC的剪裁区域
//
//  bRequireAlphaChannel
//      主要用于通知EDIT,RICHEDIT之类的控件，在绘制时，是否需要携带alpha通道参数    
//
RenderContext::RenderContext(const RECT*  prcInitClip, const RECT* prcCurrentDraw, bool bUpdateClip)
{
    UIASSERT(prcInitClip && prcCurrentDraw);

    m_nClipRectCount = 1;
    m_prcInitClip = new RECT[m_nClipRectCount];
    ::CopyRect(m_prcInitClip, prcInitClip);
    ::CopyRect(&m_rcCurrentClip, prcCurrentDraw);
    
    m_ptOffset.x = m_ptOffset.y = 0;
    m_ptBufferOffset.x = m_ptBufferOffset.y = 0;

    m_lFlags = 0;
    m_bUpdateClip = bUpdateClip;
}
RenderContext::RenderContext(const RECT*  prcInitClipArray, unsigned int nCount, const RECT* prcCurrentDraw, bool bUpdateClip)
{
    UIASSERT(prcInitClipArray && prcCurrentDraw && nCount > 0);

    m_nClipRectCount = nCount;
    m_prcInitClip = new RECT[m_nClipRectCount];
    for (unsigned int i = 0; i < nCount; i++)
        ::CopyRect(&m_prcInitClip[i], &prcInitClipArray[i]);
    
    ::CopyRect(&m_rcCurrentClip, prcCurrentDraw);
    
    m_ptOffset.x = m_ptOffset.y = 0;
    m_ptBufferOffset.x = m_ptBufferOffset.y = 0;

    m_lFlags = 0;
    m_bUpdateClip = bUpdateClip;
}
RenderContext::RenderContext(const RenderContext& o)
{
    m_nClipRectCount = o.m_nClipRectCount;
    m_prcInitClip = new RECT[m_nClipRectCount];
    for (unsigned int i = 0; i < m_nClipRectCount; i++)
        ::CopyRect(&m_prcInitClip[i], &o.m_prcInitClip[i]);

    ::CopyRect(&m_rcCurrentClip, &o.m_rcCurrentClip);

    m_ptOffset.x = o.m_ptOffset.x;
    m_ptOffset.y = o.m_ptOffset.y;
    m_ptBufferOffset.x = o.m_ptBufferOffset.x;
    m_ptBufferOffset.y = o.m_ptBufferOffset.y;

    m_lFlags = o.m_lFlags;
    m_bUpdateClip = o.m_bUpdateClip;
}
RenderContext::~RenderContext()
{
    SAFE_ARRAY_DELETE(m_prcInitClip);
    m_nClipRectCount = 0;
}

void  RenderContext::InitClip(IRenderTarget* pRenderTarget, bool bClear)
{
    if (!m_nClipRectCount)
        return;

    // 清空缓存
    if (bClear)
    {
//         HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
//         HDC hDC = pRenderTarget->GetHDC();

        CRect  rc;
        for (unsigned int i = 0; i < m_nClipRectCount; i++)
        {
            rc.CopyRect(&m_prcInitClip[i]);
            rc.OffsetRect(m_ptBufferOffset.x, m_ptBufferOffset.y);
            pRenderTarget->Clear(0, &rc);
//            ::FillRect(hDC, &rc, hBrush);
        }   
    }

    // 剪裁
    HRGN hRgn = NULL;
    if (m_nClipRectCount == 1)
    {
        CRect rc = m_prcInitClip[0];
        rc.OffsetRect(m_ptBufferOffset.x, m_ptBufferOffset.y);
        hRgn = CreateRectRgnIndirect(&rc);
    }
    else
    {
        RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + m_nClipRectCount*sizeof(RECT) ];
        pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
        pRgnData->rdh.iType      = RDH_RECTANGLES;
        pRgnData->rdh.nCount     = m_nClipRectCount;
        pRgnData->rdh.nRgnSize   = m_nClipRectCount*sizeof(RECT);
        for (unsigned int k = 0; k < m_nClipRectCount; k++)
        {
            RECT* prc = (RECT*)pRgnData->Buffer;
            prc[k] = m_prcInitClip[k];
            ::OffsetRect(&prc[k], m_ptBufferOffset.x, m_ptBufferOffset.y);
        }
        hRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + m_nClipRectCount*sizeof(RECT), pRgnData);
        SAFE_ARRAY_DELETE(pRgnData);
    }
   
    if (hRgn)
    {
        pRenderTarget->SelectClipRgn(hRgn);
        SAFE_DELETE_GDIOBJECT(hRgn);
    }
}


// 比较init与current
HRGN  RenderContext::CreateCurrentClipRegion()
{
    if (1 == m_nClipRectCount)
    {
        RECT  rcClip;
        if (::IntersectRect(&rcClip, &m_rcCurrentClip, &m_prcInitClip[0]))
        {
            OffsetRect(&rcClip, m_ptBufferOffset.x, m_ptBufferOffset.y);
            return CreateRectRgnIndirect(&rcClip);
        }		
        else
        {
            return NULL;
        }
    }

    vector<RECT>  v;
    for (unsigned int i = 0; i < m_nClipRectCount; i++)
    {
        RECT  rcClip;
        if (!::IntersectRect(&rcClip, &m_rcCurrentClip, &m_prcInitClip[i]))
            continue;

        OffsetRect(&rcClip, m_ptBufferOffset.x, m_ptBufferOffset.y);
        v.push_back(rcClip);
    }

    if (!v.size())
        return NULL;

    if (v.size() == 1)
    {
        return CreateRectRgnIndirect(&v[0]);
    }

    RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + v.size()*sizeof(RECT) ];
    pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
    pRgnData->rdh.iType      = RDH_RECTANGLES;
    pRgnData->rdh.nCount     = v.size();
    pRgnData->rdh.nRgnSize   = v.size()*sizeof(RECT);
    for (unsigned int k = 0; k < v.size(); k++)
    {
        RECT* prc = (RECT*)pRgnData->Buffer;
        prc[k] = v[k];
    }
    HRGN hRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + v.size()*sizeof(RECT), pRgnData);
    SAFE_ARRAY_DELETE(pRgnData);

    return hRgn;
}

bool RenderContext::Update(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return false;

	pRenderTarget->SetViewportOrgEx(
        m_ptOffset.x + m_ptBufferOffset.x, 
        m_ptOffset.y + m_ptBufferOffset.y, NULL);

	if (m_bUpdateClip)
	{
        HRGN  hRgn = CreateCurrentClipRegion();
        if (!hRgn)
            return false;

        pRenderTarget->SelectClipRgn(hRgn);
        SAFE_DELETE_GDIOBJECT(hRgn);
    }
    return true;
}

void RenderContext::DrawClient(IObject* pObject)
{
    if (NULL == pObject)
        return;

    // client region
    CRegion4 r;
    pObject->GetNonClientRegion(&r);
    m_ptOffset.x += r.left;
    m_ptOffset.y += r.top;

    m_rcCurrentClip.left += r.left;
    m_rcCurrentClip.top += r.top;
    m_rcCurrentClip.right -= r.right;
    m_rcCurrentClip.bottom -= r.bottom;

    // Scroll
    int xOffset = 0, yOffset = 0;
    if (pObject->GetScrollOffset(&xOffset, &yOffset))
    {
        m_ptOffset.x -= xOffset;
        m_ptOffset.y -= yOffset;
    }
}

//////////////////////////////////////////////////////////////////////////
// 返回false时，表示这个对象已完全超出当前剪裁区域了，不需要再绘制
bool RenderContext::DrawChild(IObject* pChild)
{
    if (NULL == pChild)
        return false;

    CRect rcInWindow;
    CRect rcParent;

    pChild->GetParentRect(&rcParent);
    rcInWindow.CopyRect(&rcParent);
    rcInWindow.OffsetRect(m_ptOffset.x, m_ptOffset.y);

    BOOL  b = ::IntersectRect(&m_rcCurrentClip, &m_rcCurrentClip, &rcInWindow);

    m_ptOffset.x += rcParent.left;
    m_ptOffset.y += rcParent.top;

    return b?true:false;
}

bool  RenderContext::DrawListItem(IListItemBase* pItem)
{
	if (NULL == pItem)
		return false;

	CRect rcParent;
    CRect rcIntersect;
    CRect rcInWindow;

	pItem->GetParentRect(&rcParent);
    rcInWindow.CopyRect(&rcParent);
    rcInWindow.OffsetRect(m_ptOffset.x, m_ptOffset.y);

    BOOL  b = ::IntersectRect(&m_rcCurrentClip, &m_rcCurrentClip, &rcInWindow);

	m_ptOffset.x += rcParent.left;
	m_ptOffset.y += rcParent.top;

    return b?true:false;
}

void  RenderContext::ModifyFlags(long lAdd, long lRemove)
{
    if (lAdd)
        m_lFlags |= lAdd;
    if (lRemove)
        m_lFlags &= ~lRemove;
}

bool  RenderContext::TestFlags(long lFlag)
{
    if ((m_lFlags & lFlag) == lFlag)
        return true;

    return false;
}
}