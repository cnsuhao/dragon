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

// [注]: 需要外部显示调用刷新
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

		// 使用Window的rendertarget创建其它RT
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


// 不更新自己，将自己提单到窗口上
void  RenderLayer::DirectCommit(bool bUpdate)
{
	EnableUpdateLayer(false);
	m_pObjCreate->UpdateObject(bUpdate);   // 直接借用UpdateObject。但UpdateObject中会多走一次无效的UpdateLayer来刷新自己
	EnableUpdateLayer(true);
}

// 将内容提交到pRenderTarget上。pRenderTarget已设置了偏移和剪裁
void  RenderLayer::Commit(IRenderTarget*  pRenderTarget)
{
	// 由gpu合成
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


// 1. 枚举需要刷新的祖先
// 2. 计算是否需要清空layer区域
// 3. 计算在每个祖先上的可见区域，以及在层上的可见区域
void  GetObjectUpdateContext(
		__in  Object*  pObjDamage, 
		__in  LPCRECT prcObjDamage, 
		__out deque<AncestorDamageInfo>&  stackContext,
		__out RECT*   prcVisibleInLayer,
		__out bool*   pbNeedClearLayer,
		__out POINT*  ptVisibleRectOffset=NULL // 计算最后的prcVisibleInLayer相对于prcObjDamage有多少偏移
)
{
    bool bNeedDrawBkgnd = true;
    if (!pObjDamage->IsTransparent())
        bNeedDrawBkgnd = false;
    
    // 用于计算在层上的剪裁区域
    RECT  rcClip;
    RECT  rcObj = {0, 0, pObjDamage->GetWidth(), pObjDamage->GetHeight()};
    if (prcObjDamage)
    {
        // 例如listitem可以只露出来一半，计算出来的itemrect就会超出控件范围
        // 因此在这里做一次限制
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

        // 计算在该父控件中的脏区域
        if (pObjChild->IsNcObject())
        {
            // 子对象在父控件中的范围
            Object::ChildRect2ParentRect(pObjChild, &rcClip, &rcClip);

            // 父控件中范围
            CRect  rcParent(0, 0, pObjParent->GetWidth(), pObjParent->GetHeight());

            // 求交
            IntersectRect(&data.rcDamage, &rcClip, &rcParent); 
            
            if (ptVisibleRectOffset)
            {
                ptVisibleRectOffset->x += rcClip.left - data.rcDamage.left;
                ptVisibleRectOffset->y += rcClip.top - data.rcDamage.top;
            }
        }
        else
        {
            // 子对象在父控件中的范围
            Object::ChildRect2ParentClientRect(pObjChild, &rcClip, &rcClip);

            // 父控件中范围
            CRect  rcParent;
            pObjParent->GetClientRectInObject(&rcParent);
            Object::ObjectRect2ObjectClientRect(pObjParent, &rcParent, &rcParent);

            // 求交
            IntersectRect(&data.rcDamage, &rcClip, &rcParent); 

            if (ptVisibleRectOffset)
            {
                ptVisibleRectOffset->x += rcClip.left - data.rcDamage.left;
                ptVisibleRectOffset->y += rcClip.top - data.rcDamage.top;
            }

            // 用于下一次计算
            Object::ObjectClientRect2ObjectRect(pObjParent, &data.rcDamage, &rcClip);
        }
        stackContext.push_back(data);

        // 该祖先是不透明的，则不再需要更上层的祖先去绘制背景了
        if (bNeedDrawBkgnd && !pObjParent->IsTransparent()) 
        {
            bNeedDrawBkgnd = false;
        }
        if (pObjParent->GetSelfRenderLayer2())
        {
            // 保存最后的剪裁区域
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

		// 有可能prcObjDamageArray就超出了控件，因此需要做一个并集
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

// 更新该层。直接刷新m_pObjCreate即可
// 当rcDamage为NULL时，刷新整个层区域
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

		// layer中的第一次update即会设置剪裁区，因此InitClipRegion中不需要剪裁
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

// 刷新本层上的一个子对象
bool  RenderLayer::UpdateDescendantObject(
		Object*  pObjDamage,
		LPCRECT prcObjDamageArray,
		int nCount, 
		RECT* prcOutDamageInLayerArray
)
{
    //////////////////////////////////////////////////////////////////////////
    // 1. 判断是刷新自己
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
    // 2. 刷新参数计算
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
    // 3. 刷新
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

	// 将无效区域进行变换。例如按钮旋转了90度，则commit rect也需要修改
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

// 窗口开启/禁用硬件合成
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

	// 有可能该transform是后创建的，可能没有响应到onsize通知。因此再调用一次
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