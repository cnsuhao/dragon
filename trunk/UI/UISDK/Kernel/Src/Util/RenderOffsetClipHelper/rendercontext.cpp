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
RenderContext::RenderContext(RECT* prcRenderRegion, bool bClip, bool bRequireAlphaChannel)
{
	m_ptOffset.x = 0;
	m_ptOffset.y = 0;

    if (prcRenderRegion)
    {
        ::CopyRect(&m_rcDrawRegion, prcRenderRegion);
    }
	else
	{
        ::SetRectEmpty(&m_rcDrawRegion);
	}
    m_bUpdateClip = bClip;
    m_bRequireAlphaChannel = bRequireAlphaChannel;
}
void RenderContext::Reset(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;

	pRenderTarget->SelectClipRgn(NULL);
	pRenderTarget->SetViewportOrgEx(0,0,NULL);

    m_ptOffset.x = m_ptOffset.y = 0;
    ::SetRectEmpty(&m_rcDrawRegion);
}
void RenderContext::Update(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;

	pRenderTarget->SetViewportOrgEx(m_ptOffset.x, m_ptOffset.y, NULL);

	if (m_bUpdateClip)
	{
		HRGN hRgn = CreateRectRgnIndirect(&m_rcDrawRegion);
		pRenderTarget->SelectClipRgn(hRgn);
		SAFE_DELETE_GDIOBJECT(hRgn);
	}
}

void RenderContext::DrawClient(IObject* pObject)
{
    if (NULL == pObject)
        return;

    CRegion4 r;
    pObject->GetNonClientRegion(&r);

    m_ptOffset.x += r.left;
    m_ptOffset.y += r.top;

    if (m_bUpdateClip)
    {
        m_rcDrawRegion.left -= r.left;
        m_rcDrawRegion.top -= r.top;
        m_rcDrawRegion.right -= r.right;
        m_rcDrawRegion.bottom -= r.bottom;
    }
}

void RenderContext::Scroll(IObject* pObjScroll)
{
	if (NULL == pObjScroll)
		return;
	
	int xOffset = 0, yOffset = 0;
	if (pObjScroll->GetScrollOffset(&xOffset, &yOffset))
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

    CRect rcParent;
    pChild->GetParentRect(&rcParent);

    if (m_bUpdateClip)
    { 
        CRect rcIntersect;
        CRect rcInWindow(&rcParent);
        rcInWindow.OffsetRect(m_ptOffset.x, m_ptOffset.y);

        if (!rcIntersect.IntersectRect(&m_rcDrawRegion, &rcInWindow))
        {
            ::SetRectEmpty(&m_rcDrawRegion);
            return false;
        }

        m_rcDrawRegion = rcIntersect;
    }

    m_ptOffset.x += rcParent.left;
    m_ptOffset.y += rcParent.top;

    return true;
}

bool  RenderContext::DrawListItem(IListItemBase* pItem)
{
	if (NULL == pItem)
		return false;

	CRect rcParent;
	pItem->GetParentRect(&rcParent);

	if (m_bUpdateClip)
	{ 
		CRect rcIntersect;
		CRect rcInWindow(&rcParent);
		rcInWindow.OffsetRect(m_ptOffset.x, m_ptOffset.y);

		if (!rcIntersect.IntersectRect(&m_rcDrawRegion, &rcInWindow))
		{
            ::SetRectEmpty(&m_rcDrawRegion);
			return false;
		}

		m_rcDrawRegion = rcIntersect;
	}

	m_ptOffset.x += rcParent.left;
	m_ptOffset.y += rcParent.top;

	return true;
}


}