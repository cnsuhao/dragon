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

    // ������֮��m_prcArray�Ĵ�С��m_nCount���ܲ�ƥ��
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
    // ע��m_nCountС�ڵ���m_prcArray��ʵ�ʴ�С����IntersectRect
	RECT*  m_prcArray;
	uint   m_nCount;
};

// ���λ��Ʋ��õ����Զ����µĻ��Ʒ�ʽ����InvalidateRect����
// �Զ����µĻ���ģʽ�У���������RenderLayer����ҪUpdate����ʹ�û���
#define RENDERCONTEXT_FLAG_RENDER_MODE_TOPDOWN   0x0001

/*
 * Q1�� Ϊʲô��Ҫά��һ�� CURRENT DRAW RECT
 * A1�� ����ʵ��OnPaintˢ��(�Զ����»���)�������жϣ�������Ʋ�����Ч�����ڵĿؼ�
 * 
 * Q2�� CURRENT DRAW RECT��parent���꣬����layer����
 * A2:  layer���ꡣ���ڽ�����Ϊ��������ѡ��DC���У�ʹ��parent�����û��������ˣ�
 *      �ӿؼ����ж��Լ��Ƿ��ڼ��������еķ����ǣ����Լ�parent rect��ƫ�ƣ����RECT������
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
    // �Ƿ���Ҫά��CLIP����
	bool   m_bUpdateClip;  

    // ��¼��ǰ���ڻ��ƵĿؼ������ֻ꣬�е�m_bUpdateClipΪtrueʱ��
    // �ŻὫm_curDrawRegionInLayer��InitClip�Ľ���ѡ��rendertarget
    RECT  m_curDrawRegionInLayer;  

    // ��¼��ǰ��HDCƫ�������ؼ���parent����ƫ��m_ptOffset��Ϊlayer����
    POINT  m_ptOffset;       
	
    // ֻ�����ڻ����е�ƫ��������rendercontext�У��ؼ��Ը�ƫ���޸�֪��
    //����һ�����⣬���Ǵ���Commit����RenderLayer2::Commit��
    // 2014.3.26 ���Ӳ������ڻ����е�ƫ�ơ����ϵ����ݲ�һ����ֱ�Ӿ�
    // ��Ⱦ�������(0,0)λ�á���RenderLayer2::m_rcDrawInBuffer��Ӧ
    POINT  m_ptBufferOffset; 

	long    m_lFlags;

private:
    // ֧��һ��ˢ�¶����������listctrl
    uint  m_nClipRectCount;  

    // ���ó�ʼ�������򣨼��Լ�Ҫˢ�µ����򣩡�
    // �����������ߵ��������ߵ��Լ�ʱ����m_bUpdateClip����Ϊtrue��
    // ���������ڻ����Լ����Լ����Ӷ���ʱ�����Ӷ���
    RECT*   m_prcClipRegionInLayer;   
};

}

