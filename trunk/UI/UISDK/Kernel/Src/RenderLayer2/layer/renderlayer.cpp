#include "stdafx.h"
#include "renderlayer.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3d\renderlayersoft3d.h"
#include "UISDK\Kernel\Src\RenderLayer2\affinetransform\affinetransform.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3dtransform\soft3dtransform.h"

using namespace UI;


RenderLayer::RenderLayer(Object* pObj)
{
    m_lFlags = RENDERLAYER2_FLAG_NEED_UPDATE;
    m_pObjCreate = pObj;
    m_pWindowRender = pObj->GetWindowRender();
	m_nOpcity = 255;
    m_pRenderTarget = NULL;
	m_pIRenderLayer2 = NULL;
	m_pTransform2 = NULL;
    m_rcDrawInBuffer.SetRectEmpty();
    m_eRotateCenterType = TRANSFORM_ROTATE_ORIGIN_CENTER;
    m_ptRotateCenterOffset.x = m_ptRotateCenterOffset.y = 0;
    m_pGpuTexture = NULL;
}

RenderLayer::~RenderLayer()
{
    if (m_pWindowRender)
    {
        m_pWindowRender->OnRenderLayerDestroy(this);
    }
	SAFE_DELETE(m_pIRenderLayer2);
    SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pTransform2);
    SAFE_RELEASE(m_pGpuTexture);
}

IRenderLayer*  RenderLayer::GetIRenderLayer2()
{
	if (!m_pIRenderLayer2)
	{
		m_pIRenderLayer2 = new IRenderLayer(this);
	}
	return m_pIRenderLayer2;
}

RenderLayer*  RenderLayer::GetParentRenderLayer()
{
	Object*  pParent = m_pObjCreate->GetParentObject();
	if (!pParent)
		return NULL;

	return pParent->GetRenderLayer2();
}

void  RenderLayer::EnableUpdateLayer(bool b)
{
    if (b)
        m_lFlags &= ~RENDERLAYER2_DISABLE_UPDATELAYER;
    else
        m_lFlags |= RENDERLAYER2_DISABLE_UPDATELAYER;
}

bool  RenderLayer::CanUpdateLayer()
{
    if (m_lFlags & RENDERLAYER2_DISABLE_UPDATELAYER)
        return false;

    return true;
}

Object*  RenderLayer::GetCreateObject() 
{
    return m_pObjCreate; 
}

byte  RenderLayer::GetOpacity()
{ 
	return m_nOpcity; 
}

// [ע]: ��Ҫ�ⲿ��ʾ����ˢ��
void  RenderLayer::SetOpacity(byte b)
{
	if (m_nOpcity == b)
		return;

	m_nOpcity = b; 
}

bool  RenderLayer::IsTransparent()
{
	return m_pObjCreate->IsTransparent();
}

IRenderTarget*  RenderLayer::GetRenderTarget()
{
	if (!m_pRenderTarget)        
	{
		m_pWindowRender->CreateRenderTarget(&m_pRenderTarget);
		if (!m_pRenderTarget)
			return NULL;

		// ʹ��Window��rendertarget��������RT
		RenderLayer*  pWindowRenderLayer = m_pWindowRender->m_pWindow->GetSelfRenderLayer2();
		if (!pWindowRenderLayer)
			return NULL;

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


// �������Լ������Լ��ᵥ��������
void  RenderLayer::DirectCommit(bool bUpdate)
{
	EnableUpdateLayer(false);
	m_pObjCreate->UpdateObject(bUpdate);   // ֱ�ӽ���UpdateObject����UpdateObject�л����һ����Ч��UpdateLayer��ˢ���Լ�
	EnableUpdateLayer(true);
}

// �������ύ��pRenderTarget�ϡ�pRenderTarget��������ƫ�ƺͼ���
void  RenderLayer::Commit(IRenderTarget*  pRenderTarget)
{
	// ��gpu�ϳ�
	if (IsHardComposite())
		return;

	if (!pRenderTarget)
		return;

	if (m_nOpcity == 0)
		return;

#ifdef TRACE_DRAW_PROCESS
	UI_LOG_DEBUG(_T("CommitLayer: %s"), m_pObjCreate->GetId());
#endif

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

	MATRIX33  matrix2d;
	MATRIX44  matrix3d;
	if (m_pTransform2)
	{
		switch (m_pTransform2->get_type())
		{
		case TRANSFORM_2D:
			{
				static_cast<IRenderLayerTransform2D*>(m_pTransform2)->get_matrix(&matrix2d);
				param.pTransform2d = &matrix2d;
			}
			break;

		case TRANSFORM_SOFT3D:
		case TRANSFORM_HARD3D:
			{
                IRenderLayerTransform3D*  pTransform3d = static_cast<IRenderLayerTransform3D*>(m_pTransform2);
                if (!pTransform3d->is_visible())
                    return;

                pTransform3d->get_matrix(&matrix3d);
				param.pTransform3d = &matrix3d;
			}
			break;
		}
	}

	GetRenderTarget()->Render2Target(pRenderTarget, &param);
}

IGpuLayerTexture*  RenderLayer::GetGpuTexture()
{
    return m_pGpuTexture;
}


void  RenderLayer::OnSize(uint nWidth, uint nHeight)
{
    if (!m_pRenderTarget)
    {
        GetRenderTarget();
    }
	if (m_pRenderTarget->ResizeRenderBuffer(nWidth, nHeight))
	{
		m_rcDrawInBuffer.SetRect(0, 0, nWidth, nHeight);
	}
        
	if (m_pTransform2)
	{
		m_pTransform2->set_size(nWidth, nHeight);
	}
    if (m_pGpuTexture)
    {
        m_pGpuTexture->Resize(nWidth, nHeight);
    }

    SetNeedUpdate(true);
}


// 1. ö����Ҫˢ�µ�����
// 2. �����Ƿ���Ҫ���layer����
// 3. ������ÿ�������ϵĿɼ������Լ��ڲ��ϵĿɼ�����
void  GetObjectUpdateContext(
		__in  Object*  pObjDamage, 
		__in  LPCRECT prcObjDamage, 
		__out deque<AncestorDamageInfo>&  stackContext,
		__out RECT*   prcVisibleInLayer,
		__out bool*   pbNeedClearLayer,
		__out POINT*  ptVisibleRectOffset=NULL // ��������prcVisibleInLayer�����prcObjDamage�ж���ƫ��
)
{
    bool bNeedDrawBkgnd = true;
    if (!pObjDamage->IsTransparent())
        bNeedDrawBkgnd = false;
    
    // ���ڼ����ڲ��ϵļ�������
    RECT  rcClip;
    RECT  rcObj = {0, 0, pObjDamage->GetWidth(), pObjDamage->GetHeight()};
    if (prcObjDamage)
    {
        // ����listitem����ֻ¶����һ�룬���������itemrect�ͻᳬ���ؼ���Χ
        // �����������һ������
        IntersectRect(&rcClip, &rcObj, prcObjDamage);
    }
    else
    {
        CopyRect(&rcClip, &rcObj);  
    }

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
            pObjParent->GetClientRectInObject(&rcParent);
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
        stackContext.push_back(data);

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
void  GetObjectUpdateContextArray(
		__in  Object*  pObjDamage, 
		__in  LPCRECT prcObjDamageArray, 
		__in  int  nCount,
		__out deque<AncestorDamageInfo>&  stackContext,
		__out RECT*  prcVisibleInLayer,
		__out int* pnRealCount,
		__out bool*  pbNeedClearLayerArray
)
{
    RECT  rcVisibleInLayer = {0};
    RECT  rcObj = {0, 0, pObjDamage->GetWidth(), pObjDamage->GetHeight()};
    POINT  ptClipOffset = {0, 0};

    GetObjectUpdateContext(pObjDamage, &rcObj, stackContext, &rcVisibleInLayer, pbNeedClearLayerArray, &ptClipOffset);
    POINT  ptOffset = {ptClipOffset.x + rcVisibleInLayer.left, ptClipOffset.y + rcVisibleInLayer.top};

    RECT  rcOffset;
    int nRealCount = 0;
    for (int i= 0; i < nCount; i++)
    {
        CopyRect(&rcOffset, &prcObjDamageArray[i]);
        ::OffsetRect(&rcOffset, ptOffset.x, ptOffset.y);

		// �п���prcObjDamageArray�ͳ����˿ؼ��������Ҫ��һ������
		IntersectRect(&rcOffset, &rcOffset, &rcVisibleInLayer);
		prcVisibleInLayer[nRealCount] = rcOffset;
           
        nRealCount++;
    }
    if (pnRealCount)
    {
        *pnRealCount = nRealCount;
    }
}

bool  RenderLayer::NeedUpdate()
{
	return (
        m_lFlags & 
            (RENDERLAYER_FLAG_ALWRAYS_UPDATE|
            RENDERLAYER2_FLAG_NEED_UPDATE)) ?
        true:false;
}

void  RenderLayer::SetNeedUpdate(bool b)
{
	if (b)
		m_lFlags |= RENDERLAYER2_FLAG_NEED_UPDATE;
	else
		m_lFlags &= ~RENDERLAYER2_FLAG_NEED_UPDATE;
}
void  RenderLayer::SetAlwaysUpdate(bool b)
{
    if (b)
        m_lFlags |= RENDERLAYER_FLAG_ALWRAYS_UPDATE;
    else
        m_lFlags &= ~RENDERLAYER_FLAG_ALWRAYS_UPDATE;
}

// ���¸ò㡣ֱ��ˢ��m_pObjCreate����
// ��rcDamageΪNULLʱ��ˢ������������
void  RenderLayer::UpdateLayer(LPCRECT prcDamage)
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

void  RenderLayer::UpdateLayer(LPCRECT prcDamageArray, int nCount)
{
    if (!CanUpdateLayer())
        return;
	SetNeedUpdate(false);

#ifdef TRACE_DRAW_PROCESS
	UI_LOG_DEBUG(_T("UpdateLayer: %s"), m_pObjCreate->GetId());
#endif

    RectArray  rcInitClipArray;
    rcInitClipArray.CopyFromArray(prcDamageArray, nCount);

    RECT  rcCurrentDraw;
    rcInitClipArray.GetUnionRect(&rcCurrentDraw);

    IRenderTarget*  pRenderTarget = GetRenderTarget();
    pRenderTarget->BeginDraw();
    {    
        RenderContext context(
            rcInitClipArray.m_prcArray, 
            nCount, 
            &rcCurrentDraw, 
            m_pObjCreate->NeedClip());

        context.m_ptBufferOffset = m_rcDrawInBuffer.TopLeft();		
		context.m_lFlags |= RENDERCONTEXT_FLAG_RENDER_MODE_TOPDOWN;

		// layer�еĵ�һ��update�������ü����������InitClipRegion�в���Ҫ����
        context.InitClipRegion(pRenderTarget, true, false);  
        context.Update(pRenderTarget);

        m_pObjCreate->RealDrawObject(pRenderTarget, context);
    }
    pRenderTarget->EndDraw();

	if (m_pGpuTexture)
	{
		pRenderTarget->Upload2Gpu(m_pGpuTexture, (LPRECT)prcDamageArray, nCount);
	}
}

// ˢ�±����ϵ�һ���Ӷ���
bool  RenderLayer::UpdateDescendantObject(
		Object*  pObjDamage,
		LPCRECT prcObjDamageArray,
		int nCount, 
		RECT* prcOutDamageInLayerArray
)
{
    //////////////////////////////////////////////////////////////////////////
    // 1. �ж���ˢ���Լ�
    //////////////////////////////////////////////////////////////////////////
    if (pObjDamage == m_pObjCreate)
    {
        UpdateLayer(prcObjDamageArray, nCount);

        for (int i = 0; i < nCount; i++)
        {
            CopyRect(&prcOutDamageInLayerArray[i], &prcObjDamageArray[i]);

			if (m_pTransform2)
			{
				Quad q;
				RECT& rc = prcOutDamageInLayerArray[i];

				m_pTransform2->maprect_layer_2_view(&rc, &q);
				q.GetBoundRect(&rc);
			}
        }
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    // 2. ˢ�²�������
    //////////////////////////////////////////////////////////////////////////

    bool  bNeedClearLayer = false;
    int   nRealCount = 1;
    deque<AncestorDamageInfo>  stackContext;
	RECT  rcCurrentClip = {0, 0, m_pObjCreate->GetWidth(), m_pObjCreate->GetHeight()};

    if (nCount == 1)
    {
        GetObjectUpdateContext(
            pObjDamage, 
            prcObjDamageArray, 
            stackContext, 
            prcOutDamageInLayerArray,
            &bNeedClearLayer);

        if (IsRectEmpty(prcOutDamageInLayerArray))
            return false;

		rcCurrentClip = *prcOutDamageInLayerArray;
    }
    else
    {
        GetObjectUpdateContextArray(
            pObjDamage, 
            prcObjDamageArray, 
            nCount, 
            stackContext, 
            prcOutDamageInLayerArray, 
            &nRealCount, 
            &bNeedClearLayer);

        if (0 == nRealCount)
            return false;

		for (int i = 0; i < nRealCount; i++)
		{
			UnionRect(&rcCurrentClip, &rcCurrentClip, &prcOutDamageInLayerArray[i]);
		}
    }

    //////////////////////////////////////////////////////////////////////////
    // 3. ˢ��
    //////////////////////////////////////////////////////////////////////////

    IRenderTarget*  pRenderTarget = GetRenderTarget();

    pRenderTarget->BeginDraw();
    {
        RenderContext   rendercontext(
			prcOutDamageInLayerArray, nRealCount, 
			&rcCurrentClip, false);

        rendercontext.m_ptBufferOffset = m_rcDrawInBuffer.TopLeft();
        rendercontext.InitClipRegion(pRenderTarget, bNeedClearLayer);
        rendercontext.Update(pRenderTarget);

        m_pObjCreate->HandleDamageStackContext(pObjDamage, m_pRenderTarget, 
            rendercontext, stackContext);
    }
    pRenderTarget->EndDraw();

    // gpu
    if (m_pGpuTexture)
	    pRenderTarget->Upload2Gpu(m_pGpuTexture, prcOutDamageInLayerArray, nRealCount);

	// ����Ч������б任�����簴ť��ת��90�ȣ���commit rectҲ��Ҫ�޸�
	if (m_pTransform2)
	{
		for (int i = 0; i < nRealCount; i++)
		{
			Quad q;
			RECT& rc = prcOutDamageInLayerArray[i];
			
			m_pTransform2->maprect_layer_2_view(&rc, &q);
			q.GetBoundRect(&rc);
		}
	}

    return true;
}



void  RenderLayer::GetRectDrawInBuffer(RECT* prc)
{
    CopyRect(prc, &m_rcDrawInBuffer);
}
void  RenderLayer::GetOffsetDrawInBuffer(POINT* ppt)
{
    ppt->x = m_rcDrawInBuffer.left;
    ppt->y = m_rcDrawInBuffer.top;
}

void  RenderLayer::ClearChildren()
{
    m_vecChild.clear();
}
void  RenderLayer::AddChild(RenderLayer* pChild)
{
    m_vecChild.push_back(pChild);
}
void  RenderLayer::Compositor2Gpu(GpuLayerCommitContext context)
{
    if (m_pGpuTexture)
    {
		RECT rcWnd;
		m_pObjCreate->GetWindowRect(&rcWnd);
		context.SetOffset(rcWnd.left, rcWnd.top);

		m_pGpuTexture->Compositor(&context, m_pTransform2);
    }

    _ChildIter iter = m_vecChild.begin();
    for (iter ; iter != m_vecChild.end(); ++iter)
    {
        (*iter)->Compositor2Gpu(context);
    }
}

// ���ڿ���/����Ӳ���ϳ�
void  RenderLayer::OnHardCompositeChanged(bool bEnable)
{
	if (bEnable)
	{
		if (!m_pGpuTexture)
		{
			m_pGpuTexture = m_pWindowRender->CreateGpuLayerTexture(this);
			if (m_pGpuTexture)
			{
				m_pGpuTexture->Resize(m_pObjCreate->GetWidth(), m_pObjCreate->GetHeight());
			}
		}
	}
	else
	{
		if (m_pGpuTexture)
		{
			SAFE_RELEASE(m_pGpuTexture);
		}
	}

	_ChildIter iter = m_vecChild.begin();
	for (iter ; iter != m_vecChild.end(); ++iter)
	{
		(*iter)->OnHardCompositeChanged(bEnable);
	}
}

void  RenderLayer::CreateTransform(TRANSFORM_TYPE eType)
{
	if (m_pTransform2)
	{
		if (m_pTransform2->get_type() == eType)
			return;

		SAFE_RELEASE(m_pTransform2);
	}

	switch (eType)
	{
	case TRANSFORM_2D:
		m_pTransform2 = AffineTransform::CreateInstance();
		break;

	case TRANSFORM_SOFT3D:
		m_pTransform2 = Transform3D::CreateInstance();
		break;

	case TRANSFORM_HARD3D:
		{
			m_pTransform2 = CreateHard3DTransform();
		}
		break;
	}

	// �п��ܸ�transform�Ǻ󴴽��ģ�����û����Ӧ��onsize֪ͨ������ٵ���һ��
	if (m_pTransform2)
		m_pTransform2->set_size(m_pObjCreate->GetWidth(), m_pObjCreate->GetHeight());
}

bool  RenderLayer::HasTransform()
{
	return m_pTransform2 ? true:false;
}

IRenderLayerTransform2D*  RenderLayer::GetTransform2D(bool bForceCreate)
{
	if (m_pTransform2 && m_pTransform2->get_type() == TRANSFORM_2D)
		return static_cast<IRenderLayerTransform2D*>(m_pTransform2);

	if (!bForceCreate)
		return NULL;

	CreateTransform(TRANSFORM_2D);
	return static_cast<IRenderLayerTransform2D*>(m_pTransform2);
}

bool  RenderLayer::IsHardComposite()
{
	if (!m_pWindowRender || !m_pWindowRender->m_pWindow)	
		return false;

	return m_pWindowRender->m_pWindow->IsHardComposite();
}

IRenderLayerTransform3D*  RenderLayer::GetTransform3D(bool bForceCreate)
{
	if (m_pTransform2)
	{
		TRANSFORM_TYPE eType = m_pTransform2->get_type();
		if (eType == TRANSFORM_SOFT3D || eType == TRANSFORM_HARD3D)
			return static_cast<IRenderLayerTransform3D*>(m_pTransform2);
	}

	if (!bForceCreate)
		return NULL;

	CreateTransform(IsHardComposite() ? TRANSFORM_HARD3D:TRANSFORM_SOFT3D);
	return static_cast<IRenderLayerTransform3D*>(m_pTransform2);
}
IRenderLayerTransform2*  RenderLayer::GetTransformPtr()
{
	return m_pTransform2;
}