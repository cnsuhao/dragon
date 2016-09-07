#pragma once
#include <vector>

namespace UI
{
class RectArray
{
public:
	RectArray()
	{
		m_prcArray = 0;
		m_nCount = 0;
	}
	~RectArray()
	{
		Destroy();
	}

    RectArray(RectArray& o)
    {
        m_prcArray = 0;
        m_nCount = 0;
        this->CopyFrom(&o);
    }
    
    RectArray& operator=(const RectArray& o)
    {
        m_prcArray = 0;
        m_nCount = 0;
        this->CopyFrom(&o);
        return *this;
    }

	void  Destroy()
	{
		SAFE_ARRAY_DELETE(m_prcArray);
		m_nCount = 0;
	}

	void  CopyFrom(const RectArray* po)
	{
		if (!po)
			return;

		CopyFromArray(po->m_prcArray, po->m_nCount);
	}

	void  CopyFromArray(LPCRECT pArray, uint nCount)
	{
        if (nCount > m_nCount)
        {
		    Destroy();

		    if (NULL == pArray || nCount == 0)
			    return;

		    m_nCount = nCount;
		    m_prcArray = new RECT[nCount];
		    memcpy(m_prcArray, pArray, sizeof(RECT)*nCount);
        }
        else
        {
            if (NULL == pArray || nCount == 0)
            {
                Destroy();
                return;
            }

            m_nCount = nCount;
            memcpy(m_prcArray, pArray, sizeof(RECT)*nCount);
        }
	}

	void  SetSize(uint nCount)
	{
		Destroy();
		m_nCount = nCount;

		if (nCount > 0)
		{
			m_prcArray = new RECT[nCount];
			memset(m_prcArray, 0, sizeof(RECT)*nCount);
		}
	}

	bool  SetAt(uint nIndex, RECT*  pValue)
	{
		if (nIndex >= m_nCount)
			return false;
		if (!pValue)
			return false;

		CopyRect(&m_prcArray[nIndex], pValue);
		return true;
	}

	void  Offset(int x, int y)
	{
		for (uint i = 0; i < m_nCount; i++)
		{
			OffsetRect(&m_prcArray[i], x, y);
		}
	}

    // 运行完之后，m_prcArray的大小与m_nCount可能不匹配
    bool IntersectRect(RECT* prc, bool OnlyTest=false)
    {
        if (0 == m_nCount)
            return false;

        RECT temp = {0};
        UINT nNewCount = 0;

        for (UINT i = 0; i < m_nCount; i++)
        {
            if (::IntersectRect(&temp, &m_prcArray[i], prc))
            {
                if (OnlyTest)
                {
                    return true;
                }
                else
                {
                    CopyRect(&m_prcArray[nNewCount], &temp);
                    nNewCount++;
                }
            }
        }

        if (nNewCount == m_nCount)
            return true;

        if (0 == nNewCount)
        {
            if (OnlyTest)
                return false;

            Destroy();
            return false;
        }

        if (OnlyTest)
            return true;

        m_nCount = nNewCount;
        return true;
    }

    HRGN  CreateRgn()
    {
        if (0 == m_nCount || NULL == m_prcArray)
        {
            return NULL;
        }
        if (1 == m_nCount)
        {
            return CreateRectRgnIndirect(m_prcArray);
        }

        RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + m_nCount*sizeof(RECT) ];
        pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
        pRgnData->rdh.iType      = RDH_RECTANGLES;
        pRgnData->rdh.nCount     = m_nCount;
        pRgnData->rdh.nRgnSize   = m_nCount*sizeof(RECT);
        for (UINT k = 0; k < m_nCount; k++)
        {
            RECT* prcBuffer = (RECT*)pRgnData->Buffer;
            prcBuffer[k] = m_prcArray[k];
        }
        return ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + m_nCount*sizeof(RECT), pRgnData);
    }

    void  GetUnionRect(RECT* prc)
    {
        SetRectEmpty(prc);
        for (uint i = 0; i < m_nCount; i++)
        {
            UnionRect(prc, &m_prcArray[i], prc);
        }
    }

	
public:
    // 注：m_nCount小于等于m_prcArray的实际大小，见IntersectRect
	RECT*  m_prcArray;
	uint   m_nCount;
};

// 本次绘制采用的是自顶向下的绘制方式（即InvalidateRect。）
// 自顶向下的绘制模式中，所遇到的RenderLayer都需要Update，不使用缓存
#define RENDERCONTEXT_FLAG_RENDER_MODE_TOPDOWN   0x0001

/*
 * Q1： 为什么需要维护一个 CURRENT DRAW RECT
 * A1： 用于实现OnPaint刷新(自顶向下绘制)的区域判断，避免绘制不在无效区域内的控件
 * 
 * Q2： CURRENT DRAW RECT是parent坐标，还是layer坐标
 * A2:  layer坐标。用于将其作为剪裁区域选入DC当中（使用parent坐标就没这个能力了）
 *      子控件在判断自己是否处于剪裁区域中的方法是，将自己parent rect作偏移，与该RECT进行求焦
*/

class UISDKAPI RenderContext
{
public:
	RenderContext(LPCRECT  prcInitClip, LPCRECT prcCurrentDraw, bool bClip);
    RenderContext(LPCRECT  prcInitClipArray, uint nCount, LPCRECT prcRenderRegion, bool bClip);
    RenderContext(const RenderContext& o);
    ~RenderContext();

    void  DrawClient(IObject* pObject);
    bool  DrawChild(IObject* pChild);
	bool  DrawListItem(IListItemBase* pItem);
	
	void  Update(IRenderTarget* pRenderTarget);
	void  UpdateOffset(IRenderTarget* pRenderTarget);
	void  UpdateClipRgn(IRenderTarget* pRenderTarget);
    void  InitClipRegion(IRenderTarget* pRenderTarget, bool bClear, bool bClip=true);

    // flags
    void  ModifyFlags(long lAdd, long lRemove);
    bool  TestFlags(long lFlag);

protected:
//    HRGN  CreateCurrentClipRegion();

public:
    // 是否需要维护CLIP参数
	bool   m_bUpdateClip;  

    // 记录当前正在绘制的控件的坐标，只有当m_bUpdateClip为true时，
    // 才会将m_curDrawRegionInLayer与InitClip的交集选入rendertarget
    RECT  m_curDrawRegionInLayer;  

    // 记录当前的HDC偏移量，控件的parent坐标偏移m_ptOffset即为layer坐标
    POINT  m_ptOffset;       
	
    // 只将层在缓存中的偏移体现在rendercontext中，控件对该偏移无感知。
    //（有一个例外，就是窗口Commit、和RenderLayer2::Commit）
    // 2014.3.26 增加层数据在缓存中的偏移。层上的内容不一定是直接就
    // 渲染到缓存的(0,0)位置。与RenderLayer2::m_rcDrawInBuffer对应
    POINT  m_ptBufferOffset; 

	long    m_lFlags;

private:
    // 支持一次刷新多个区域，例如listctrl
    uint  m_nClipRectCount;  

    // 设置初始剪裁区域（即自己要刷新的区域）。
    // 当绘制流程走到从祖先走到自己时，将m_bUpdateClip设置为true，
    // 这样就能在绘制自己及自己的子对象时剪裁子对象
    RECT*   m_prcClipRegionInLayer;   
};

}

