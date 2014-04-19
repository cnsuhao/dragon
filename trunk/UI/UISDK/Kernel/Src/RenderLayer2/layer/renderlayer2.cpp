#include "stdafx.h"
#include "renderlayer2.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3d\renderlayersoft3d.h"
#include "UISDK\Kernel\Src\RenderLayer2\transform2d\renderlayertransform2d.h"

namespace UI
{
RenderLayer2::RenderLayer2(Object* pObj)
{
    m_lFlags = RENDER_LAYER2_DIRTY;
    m_pObjCreate = pObj;
    m_pWindowRender = pObj->GetWindowRender();
	m_nOpcity = 255;
    m_pRenderTarget = NULL;
	m_pIRenderLayer2 = NULL;
    m_pTransform = NULL;
    m_rcDrawInBuffer.SetRectEmpty();
    m_eRotateCenterType = ROTATE_CENTER_CENTER;
    m_ptRotateCenterOffset.x = m_ptRotateCenterOffset.y = 0;
}
RenderLayer2::~RenderLayer2()
{
	SAFE_DELETE(m_pIRenderLayer2);
    SAFE_RELEASE(m_pRenderTarget);
	//SAFE_DELETE(m_pSoft3D);
    SAFE_DELETE(m_pTransform);
}

IRenderLayer2*  RenderLayer2::GetIRenderLayer2()
{
	if (!m_pIRenderLayer2)
	{
		m_pIRenderLayer2 = new IRenderLayer2(this);
	}
	return m_pIRenderLayer2;
}

HDC  RenderLayer2::GetHDC()
{
    if (m_pRenderTarget)
        return m_pRenderTarget->GetHDC();

    return NULL;
}
bool  RenderLayer2::IsDirty()
{
    if (m_lFlags & RENDER_LAYER2_DIRTY)
        return true;
    return false;
}

RenderLayer2*  RenderLayer2::GetParentRenderLayer()
{
	Object*  pParent = m_pObjCreate->GetParentObject();
	if (!pParent)
		return NULL;

	return pParent->GetRenderLayer2();
}

void  RenderLayer2::SetDirty(bool b)
{
	if (IsDirty() == b)
		return;

    if (b)
        m_lFlags |= RENDER_LAYER2_DIRTY;
    else
        m_lFlags &= ~RENDER_LAYER2_DIRTY;

	// ���ϵݹ�
	if (b)
	{
		RenderLayer2*  pParentLayer = GetParentRenderLayer();
		if (pParentLayer)
			pParentLayer->SetDirty(true);
	}
}

void   RenderLayer2::EnableUpdateLayer(bool b)
{
    if (b)
        m_lFlags &= ~RENDER_LAYER2_DISABLE_UPDATELAYER;
    else
        m_lFlags |= RENDER_LAYER2_DISABLE_UPDATELAYER;
}
bool   RenderLayer2::CanUpdateLayer()
{
    if (m_lFlags & RENDER_LAYER2_DISABLE_UPDATELAYER)
        return false;
    return true;
}
byte   RenderLayer2::GetOpacity()
{ 
    return m_nOpcity; 
}
Object*  RenderLayer2::GetCreateObject() 
{
    return m_pObjCreate; 
}

void  RenderLayer2::SetOpacity(byte b)
{
	m_nOpcity = b; 

	RenderLayer2*  pParentLayer = GetParentRenderLayer();
	if (pParentLayer)
		pParentLayer->SetDirty(true);
}

bool  RenderLayer2::IsTransparent()
{
    return m_pObjCreate->IsTransparent();
}

IRenderTarget*  RenderLayer2::GetRenderTarget()
{
    if (!m_pRenderTarget)        
    {
        m_pWindowRender->CreateRenderTarget(&m_pRenderTarget);

        // ʹ��Window��rendertarget��������RT
        RenderLayer2*  pWindowRenderLayer = m_pWindowRender->m_pWindow->GetSelfRenderLayer2();
        if (pWindowRenderLayer == this)
        {
            m_pRenderTarget->CreateRenderBuffer(NULL);
        }
        else
        {
            m_pRenderTarget->CreateRenderBuffer(pWindowRenderLayer->GetRenderTarget());
        }
    }
    return m_pRenderTarget;
}

void  RenderLayer2::OnSize(unsigned int nWidth, unsigned int nHeight)
{
    if (!m_pRenderTarget)
    {
        GetRenderTarget();
    }

    if (m_pTransform)
    {
        RECT rcPadding = {0};
        m_pTransform->GetBufferPadding(&rcPadding);
        int wPadding = rcPadding.left + rcPadding.right;
        int hPadding = rcPadding.top + rcPadding.bottom;
        if (m_pRenderTarget->ResizeRenderBuffer(nWidth+wPadding, nHeight+hPadding))  // ��1px�߽磬���ڶ������Բ�ֵ
        {
            m_rcDrawInBuffer.SetRect(rcPadding.left, rcPadding.top, rcPadding.left+nWidth, rcPadding.top+nHeight);
            SetDirty(true);
        }
        m_pTransform->OnSize(nWidth, nHeight);
    }
    else
    {
        if (m_pRenderTarget->ResizeRenderBuffer(nWidth, nHeight))
        {
            m_rcDrawInBuffer.SetRect(0, 0, nWidth, nHeight);
            SetDirty(true);
        }
    }
        
}


// 1. ö����Ҫˢ�µ�����
// 2. �����Ƿ���Ҫ���layer����
// 3. ������ÿ�������ϵĿɼ������Լ��ڲ��ϵĿɼ�����
void  CalcUpdateDamageObjectContext(__in  Object*  pObjDamage, 
                                    __in  const RECT* prcObjDamage, 
                                    __out stack<AncestorDamageInfo>&  stackContext,
                                    __out RECT*  prcVisibleInLayer,
                                    __out bool*  pbNeedClearLayer,
                                    __out POINT*  ptVisibleRectOffset=NULL) // ��������prcVisibleInLayer�����prcObjDamage�ж���ƫ��
{
//    UIASSERT(pObjDamage != m_pObjCreate);  // ֱ����UpdateLayer·��

    bool bNeedDrawBkgnd = true;
    if (!pObjDamage->IsTransparent())
        bNeedDrawBkgnd = false;;

    
    // ���ڼ����ڲ��ϵļ�������
    RECT  rcClip;
    if (prcObjDamage)
        CopyRect(&rcClip, prcObjDamage);
    else
        SetRect(&rcClip, 0, 0, pObjDamage->GetWidth(), pObjDamage->GetHeight());  

    Object* pObjParent = NULL;
    Object* pObjChild = pObjDamage;
    while (pObjParent = pObjDamage->EnumParentObject(pObjParent))
    {
        AncestorDamageInfo  data;
        data.pAncestor = pObjParent;
        data.bDrawBkgnd = bNeedDrawBkgnd;

        // �����ڸø��ؼ��е�������
        if (pObjChild->IsNcObject())
        {
            // �Ӷ����ڸ��ؼ��еķ�Χ
            Object::ChildRect2ParentRect(pObjChild, &rcClip, &rcClip);

            // ���ؼ��з�Χ
            CRect  rcParent(0, 0, pObjParent->GetWidth(), pObjParent->GetHeight());

            // ��
            IntersectRect(&data.rcDamage, &rcClip, &rcParent); 
            
            if (ptVisibleRectOffset)
            {
                ptVisibleRectOffset->x += rcClip.left - data.rcDamage.left;
                ptVisibleRectOffset->y += rcClip.top - data.rcDamage.top;
            }
        }
        else
        {
            // �Ӷ����ڸ��ؼ��еķ�Χ
            Object::ChildRect2ParentClientRect(pObjChild, &rcClip, &rcClip);

            // ���ؼ��з�Χ
            CRect  rcParent;
            pObjParent->GetClientRect(&rcParent);
            Object::ObjectRect2ObjectClientRect(pObjParent, &rcParent, &rcParent);

            // ��
            IntersectRect(&data.rcDamage, &rcClip, &rcParent); 

            if (ptVisibleRectOffset)
            {
                ptVisibleRectOffset->x += rcClip.left - data.rcDamage.left;
                ptVisibleRectOffset->y += rcClip.top - data.rcDamage.top;
            }

            // ������һ�μ���
            Object::ObjectClientRect2ObjectRect(pObjParent, &data.rcDamage, &rcClip);
        }
        stackContext.push(data);

        // �������ǲ�͸���ģ�������Ҫ���ϲ������ȥ���Ʊ�����
        if (bNeedDrawBkgnd && !pObjParent->IsTransparent()) 
        {
            bNeedDrawBkgnd = false;
        }
        if (pObjParent->GetSelfRenderLayer2())
        {
            // �������ļ�������
            CopyRect(prcVisibleInLayer, &rcClip);
            break;
        }
        pObjChild = pObjParent;
    }

    if (pbNeedClearLayer)
        *pbNeedClearLayer = bNeedDrawBkgnd;
}
void  CalcUpdateDamageObjectContextArray(__in  Object*  pObjDamage, 
                                    __in  const RECT* prcObjDamageArray, 
                                    __in  int  nCount,
                                    __out stack<AncestorDamageInfo>&  stackContext,
                                    __out RECT*  prcVisibleInLayer,
                                    __out int* pnRealCount,
                                    __out bool*  pbNeedClearLayerArray)
{
    RECT  rcVisibleInLayer = {0};
    RECT  rcObj = {0, 0, pObjDamage->GetWidth(), pObjDamage->GetHeight()};
    POINT  ptClipOffset = {0, 0};

    CalcUpdateDamageObjectContext(pObjDamage, &rcObj, stackContext, &rcVisibleInLayer, pbNeedClearLayerArray, &ptClipOffset);
    POINT  ptOffset = {ptClipOffset.x + rcVisibleInLayer.left, ptClipOffset.y + rcVisibleInLayer.top};

    RECT  rcOffset;
    int nRealCount = 0;
    for (int i= 0; i < nCount; i++)
    {
        CopyRect(&rcOffset, &prcObjDamageArray[i]);
        ::OffsetRect(&rcOffset, ptOffset.x, ptOffset.y);
        //if (IntersectRect(&prcVisibleInLayer[nRealCount], &rcOffset, &rcVisibleInLayer))

		// �п���prcObjDamageArray�ͳ����˿ؼ��������Ҫ��һ������
		IntersectRect(&rcOffset, &rcOffset, &rcVisibleInLayer);
		prcVisibleInLayer[nRealCount] = rcOffset;
        {
            nRealCount++;
        }
    }
    if (pnRealCount)
    {
        *pnRealCount = nRealCount;
    }
}

// ���¸ò㡣ֱ��ˢ��m_pObjCreate����
// ��rcDamageΪNULLʱ��ˢ������������
void  RenderLayer2::UpdateLayer(const RECT* prcDamage)
{
    if (prcDamage)
    {
        UpdateLayer(prcDamage, 1);
    }
    else
    {
        RECT  rcClip = {0, 0, m_pObjCreate->GetWidth(), m_pObjCreate->GetHeight()};
        UpdateLayer(&rcClip, 1);
    }

}
void  RenderLayer2::UpdateLayer(const RECT* prcDamageArray, int nCount)
{
    if (!CanUpdateLayer())
        return;

    // �ڻ�����ƫ��
    RECT* prcArray = new RECT[nCount];
    RECT  rcCurrentDraw = {0, 0, m_pObjCreate->GetWidth(), m_pObjCreate->GetHeight()};
    for (int i = 0; i < nCount; i++)
    {
        CopyRect(&prcArray[i], &prcDamageArray[i]);
    }

    IRenderTarget*  pRenderTarget = GetRenderTarget();
    pRenderTarget->BeginDraw();
    {    
        RenderContext context(prcArray, nCount, &rcCurrentDraw, true);
        context.m_ptBufferOffset = m_rcDrawInBuffer.TopLeft();
        context.InitClip(pRenderTarget, true);
        context.Update(pRenderTarget);

        m_pObjCreate->RealDrawObject(pRenderTarget, context);
    }
    pRenderTarget->EndDraw();

    SetDirty(false);
    SAFE_ARRAY_DELETE(prcArray);
}

bool  RenderLayer2::UpdateDescendant(Object*  pObjDamage, const RECT* prcObjDamageArray, int nCount, RECT* prcOutDamageInLayerArray)
{
    //////////////////////////////////////////////////////////////////////////
    // 1. ˢ���Լ�
    //////////////////////////////////////////////////////////////////////////

    if (pObjDamage == m_pObjCreate)
    {
        for (int i = 0; i < nCount; i++)
        {
            CopyRect(&prcOutDamageInLayerArray[i], &prcObjDamageArray[i]);
            if (m_pTransform)
            {
                m_pTransform->GetRelativeBoundRect(&prcOutDamageInLayerArray[i], &prcOutDamageInLayerArray[i]);
            }
        }
        UpdateLayer(prcObjDamageArray, nCount);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    // 2. ˢ�²�������
    //////////////////////////////////////////////////////////////////////////

    bool  bNeedClearLayer = false;
    stack<AncestorDamageInfo>  stackContext;
    int  nRealCount = 1;

    if (nCount == 1)
    {
        CalcUpdateDamageObjectContext(pObjDamage, prcObjDamageArray, stackContext, prcOutDamageInLayerArray, &bNeedClearLayer);
        if (IsRectEmpty(prcOutDamageInLayerArray))
            return false;
    }
    else
    {
        CalcUpdateDamageObjectContextArray(pObjDamage, prcObjDamageArray, nCount, 
            stackContext, prcOutDamageInLayerArray, &nRealCount, &bNeedClearLayer);

        if (0 == nRealCount)
            return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // 3. ˢ��
    //////////////////////////////////////////////////////////////////////////


    RECT  rcCurrentDraw = {0, 0, m_pObjCreate->GetWidth(), m_pObjCreate->GetHeight()};
    IRenderTarget*  pRenderTarget = GetRenderTarget();

    pRenderTarget->BeginDraw();
    {
        RenderContext   rendercontext(prcOutDamageInLayerArray, nRealCount, &rcCurrentDraw, false);
        rendercontext.m_ptBufferOffset = m_rcDrawInBuffer.TopLeft();
        rendercontext.InitClip(pRenderTarget, bNeedClearLayer);
        rendercontext.Update(pRenderTarget);
        m_pObjCreate->HandleDamageStackContext(pObjDamage, m_pRenderTarget, rendercontext, stackContext);
    }
    pRenderTarget->EndDraw();


    if (m_pTransform)
    {
        for (int i = 0; i < nRealCount; i++)
            m_pTransform->GetRelativeBoundRect(&prcOutDamageInLayerArray[i], &prcOutDamageInLayerArray[i]);
    }

    return true;
}


// �������Լ������Լ��ᵥ��������
void  RenderLayer2::DirectCommit(bool bUpdate)
{
    if (IsDirty())  // �п����Ǹ��ؽ���render target��û��ˢ�»���
    {
        m_pObjCreate->UpdateObject(bUpdate); 
    }
    else
    {
        EnableUpdateLayer(false);
        {
            m_pObjCreate->UpdateObject(bUpdate);   // ֱ�ӽ���UpdateObject����UpdateObject�л����һ����Ч��UpdateLayer��ˢ���Լ�
        }
        EnableUpdateLayer(true);
    }
}

// �������ύ��pRenderTarget�ϡ�pRenderTarget��������ƫ�ƺͼ���
void  RenderLayer2::Commit(IRenderTarget*  pRenderTarget)
{
    if (!pRenderTarget)
        RenderLayer2;

    Render2TargetParam  param = {0};
    param.xDst = 0;
    param.yDst = 0;
    param.wDst = m_pObjCreate->GetWidth();
    param.hDst = m_pObjCreate->GetHeight();
    param.xSrc = m_rcDrawInBuffer.left;
    param.ySrc = m_rcDrawInBuffer.top;
    param.wSrc = m_rcDrawInBuffer.right - m_rcDrawInBuffer.left;
    param.hSrc = m_rcDrawInBuffer.bottom - m_rcDrawInBuffer.top;
    param.bAlphaBlend = IsTransparent() || m_nOpcity<255;
    param.opacity = m_nOpcity;

	if (m_pTransform)
	{
		if (m_pTransform->Commit(pRenderTarget, &param))
		    return;
	}

	GetRenderTarget()->Render2Target(pRenderTarget, &param);
}

void  RenderLayer2::SetRotateCenterPos(ROTATE_CENTER_TYPE eType, int xOffset, int yOffset)
{
    m_eRotateCenterType = eType;
    m_ptRotateCenterOffset.x = xOffset;
    m_ptRotateCenterOffset.y = yOffset;
}

void  RenderLayer2::Rotate3D(int x, int y, int z)
{
    if (m_pTransform && Transform_soft3d != m_pTransform->GetTransformType())
    {
        SAFE_DELETE(m_pTransform);
    }
	if (!m_pTransform)
	{
		m_pTransform = new RenderLayer2Soft3D(this);
        this->OnSize(m_pObjCreate->GetWidth(), m_pObjCreate->GetHeight());
	}
	m_pTransform->Rotate(x, y, z);
}
void  RenderLayer2::Rotate2D(int degree)
{
    if (m_pTransform && Transform_2d != m_pTransform->GetTransformType())
    {
        SAFE_DELETE(m_pTransform);
    }
    if (!m_pTransform)
    {
        m_pTransform = new RenderLayer2Transform2d(this);
        this->OnSize(m_pObjCreate->GetWidth(), m_pObjCreate->GetHeight());
    }
    m_pTransform->Rotate(0, 0, degree);
}

void  RenderLayer2::GetRectDrawInBuffer(RECT* prc)
{
    CopyRect(prc, &m_rcDrawInBuffer);
}
void  RenderLayer2::GetOffsetDrawInBuffer(POINT* ppt)
{
    ppt->x = m_rcDrawInBuffer.left;
    ppt->y = m_rcDrawInBuffer.top;
}

bool  RenderLayer2::IsTransformed()
{
    return m_pTransform ? true:false;
}
bool  RenderLayer2::TransformPoint(POINT* pptInLayer, POINT* pptTransform)
{
    if (!m_pTransform)
        return false;

    m_pTransform->TransformPoint(pptInLayer, pptTransform);
    return true;
}

}

