#include "stdafx.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

RenderContext::RenderContext(
    LPCRECT  prcInitClip,
    LPCRECT  prcCurrentDraw, 
    bool     bUpdateClip
)
{
    UIASSERT(prcInitClip && prcCurrentDraw);

    m_nClipRectCount = 1;
    m_prcClipRegionInLayer = new RECT[m_nClipRectCount];
    ::CopyRect(m_prcClipRegionInLayer, prcInitClip);
    ::CopyRect(&m_curDrawRegionInLayer, prcCurrentDraw);
    
    m_ptOffset.x = m_ptOffset.y = 0;
    m_ptBufferOffset.x = m_ptBufferOffset.y = 0;

    m_lFlags = 0;
    m_bUpdateClip = bUpdateClip;
}

RenderContext::RenderContext(
    LPCRECT  prcInitClipArray, 
    uint     nCount, 
    LPCRECT  prcCurrentDraw,
    bool     bUpdateClip
)
{
    UIASSERT(prcInitClipArray && prcCurrentDraw && nCount > 0);

    m_nClipRectCount = nCount;
    m_prcClipRegionInLayer = new RECT[m_nClipRectCount];
    for (uint i = 0; i < nCount; i++)
        ::CopyRect(&m_prcClipRegionInLayer[i], &prcInitClipArray[i]);
    
    ::CopyRect(&m_curDrawRegionInLayer, prcCurrentDraw);
    
    m_ptOffset.x = m_ptOffset.y = 0;
    m_ptBufferOffset.x = m_ptBufferOffset.y = 0;

    m_lFlags = 0;
    m_bUpdateClip = bUpdateClip;
}

RenderContext::RenderContext(const RenderContext& o)
{
    m_nClipRectCount = o.m_nClipRectCount;
    m_prcClipRegionInLayer = new RECT[m_nClipRectCount];
    for (uint i = 0; i < m_nClipRectCount; i++)
        ::CopyRect(&m_prcClipRegionInLayer[i], &o.m_prcClipRegionInLayer[i]);

    ::CopyRect(&m_curDrawRegionInLayer, &o.m_curDrawRegionInLayer);

    m_ptOffset.x = o.m_ptOffset.x;
    m_ptOffset.y = o.m_ptOffset.y;
    m_ptBufferOffset.x = o.m_ptBufferOffset.x;
    m_ptBufferOffset.y = o.m_ptBufferOffset.y;

    m_lFlags = o.m_lFlags;
    m_bUpdateClip = o.m_bUpdateClip;
}

RenderContext::~RenderContext()
{
    SAFE_ARRAY_DELETE(m_prcClipRegionInLayer);
    m_nClipRectCount = 0;
}

//
// bClear: 是否清空剪裁区域缓存，避免alpha叠加
// bClip:  是否进行剪裁。UpdateLayer中，因为是刷新的第一步，
//         将剪裁放在Update函数中即可，因此bClip可以设置为false.
//
void  RenderContext::InitClipRegion(IRenderTarget* pRenderTarget, bool bClear, bool bClip)
{
    if (!m_nClipRectCount)
        return;

    // 清空缓存
    if (bClear)
    {
        CRect  rc;
        for (uint i = 0; i < m_nClipRectCount; i++)
        {
            rc.CopyRect(&m_prcClipRegionInLayer[i]);
            rc.OffsetRect(m_ptBufferOffset.x, m_ptBufferOffset.y);
            pRenderTarget->Clear(&rc);
        }   
    }

    // 剪裁
    if (bClip && m_nClipRectCount > 0)
    {
		RectArray array;
		array.CopyFromArray(m_prcClipRegionInLayer, m_nClipRectCount);
		array.Offset(m_ptBufferOffset.x, m_ptBufferOffset.y);
        pRenderTarget->SelectClipRect(array.m_prcArray, array.m_nCount);
    }
}



void RenderContext::Update(IRenderTarget* pRenderTarget)
{
	UpdateOffset(pRenderTarget);
	if (m_bUpdateClip)
		UpdateClipRgn(pRenderTarget);
}
void  RenderContext::UpdateOffset(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;

	pRenderTarget->SetViewportOrgEx(
		m_ptOffset.x + m_ptBufferOffset.x, 
		m_ptOffset.y + m_ptBufferOffset.y, NULL);

}
void  RenderContext::UpdateClipRgn(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;
	
	if (1 == m_nClipRectCount)
	{
		RECT  rcClip;
		if (::IntersectRect(&rcClip, &m_curDrawRegionInLayer, &m_prcClipRegionInLayer[0]))
		{
			OffsetRect(&rcClip, m_ptBufferOffset.x, m_ptBufferOffset.y);
			pRenderTarget->SelectClipRect(&rcClip, 1);
		}
	}
	else
	{
		RectArray  array;
		array.SetSize(m_nClipRectCount);

		int   nRealCount = 0;
		for (uint i = 0; i < m_nClipRectCount; i++)
		{
			RECT  rcClip;
			if (!::IntersectRect(&rcClip, &m_curDrawRegionInLayer, &m_prcClipRegionInLayer[i]))
				continue;

			OffsetRect(&rcClip, m_ptBufferOffset.x, m_ptBufferOffset.y);
			array.SetAt(nRealCount, &rcClip);
			nRealCount++;
		}
		if (nRealCount > 0)
		{
			pRenderTarget->SelectClipRect(array.m_prcArray, nRealCount);
		}
	}
    return;
}

void RenderContext::DrawClient(IObject* pIObject)
{
    UIASSERT(pIObject); 
    Object* pObject = pIObject->GetImpl();

	// 2015.2.11
	// 一般DrawClient不再更新剪裁，只更新偏移量。
	// 原因：对于一个label控件，如果文本有阴影的话，阴影区域将比文本实际区域更大。
	//       即使设置一个padding，也解决不了该问题。因为client区域被剪裁了。
	//       为了适应这种场景，对渲染重新进行修改。绘制client区域不再更新剪裁，
	//       由控件自己实现。
    //
    //      对于列表型滚动，由于要支持滚动，必须对client区域做剪裁，否则内容会滚出来
    if (pObject->NeedClipClient())
    {
        CRect rcLayer;
        pObject->GetClientRectInObject(&rcLayer);  // 当前的m_ptOffset已经定位到自己了
        rcLayer.OffsetRect(m_ptOffset.x, m_ptOffset.y);

        ::IntersectRect(&m_curDrawRegionInLayer, &m_curDrawRegionInLayer, &rcLayer);
    }

    // client region
    CRegion4 r;
    pObject->GetNonClientRegion(&r);
    m_ptOffset.x += r.left;
    m_ptOffset.y += r.top;
    
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

    CRect rcParent;
    pChild->GetParentRect(&rcParent);

    CRect rcLayer(&rcParent);
    rcLayer.OffsetRect(m_ptOffset.x, m_ptOffset.y);

    BOOL  b = ::IntersectRect(&m_curDrawRegionInLayer, &m_curDrawRegionInLayer, &rcLayer);

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

    BOOL  b = ::IntersectRect(&m_curDrawRegionInLayer, &m_curDrawRegionInLayer, &rcInWindow);

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