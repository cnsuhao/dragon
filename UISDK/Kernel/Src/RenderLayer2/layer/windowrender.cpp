#include "stdafx.h"
#include "windowrender.h"
#include "renderlayer.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Inc\Interface\iwndtransmode.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Src\Base\Attribute\attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\enum_attribute.h"

using namespace UI;

WindowRender::WindowRender(WindowBase*  p)
{
    m_eGRL = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
    m_pWindow = p;
    m_lRefCanCommit = 0;
    m_bFirstTimePaintWindow = true;

    SetCanCommit(false);
    m_pIWindowRender = NULL;
	m_pHardwareComposition = NULL;
    m_bNeedRebuildGpuLayerTree = true;
    m_bNeedAlphaChannel = true;
}

WindowRender::~WindowRender()
{
    SAFE_DELETE(m_pIWindowRender);
	SAFE_RELEASE(m_pHardwareComposition);
}

IWindowRender*  WindowRender::GetIWindowRender()
{
    if (!m_pIWindowRender)
        m_pIWindowRender = new IWindowRender(this);

    return m_pIWindowRender;
}

void  WindowRender::OnSerialize(SERIALIZEDATA* pData)
{
	AttributeSerializer s(pData, TEXT("WindowRender"));

	s.AddEnum(XML_WINDOW_GRAPHICS_RENDER_LIBRARY, *(long*)&m_eGRL)
		->FillGraphicsRenderLibraryData()
		->SetDefault(GRAPHICS_RENDER_LIBRARY_TYPE_GDI);

    s.AddBool(XML_WINDOW_NEED_ALPHACHANNEL, m_bNeedAlphaChannel);
}

RenderLayer*  WindowRender::CreateRenderLayer(Object* pObj)
{
	RenderLayer* p = new RenderLayer(pObj);
    if (m_pWindow && m_pWindow->IsHardComposite())
    {
        p->OnHardCompositeChanged(true);
    }
    return p;
}

IGpuLayerTexture*  WindowRender::CreateGpuLayerTexture(RenderLayer* p)
{
	if (NULL == m_pHardwareComposition)
	{
		if (m_pWindow->IsHardComposite())
		{
			m_pHardwareComposition = CreateHardwareComposition(m_pWindow->m_hWnd);
		}
	}

	IGpuLayerTexture* pGpuTexture = NULL;
	if (m_pHardwareComposition)
	{
		pGpuTexture = m_pHardwareComposition->CreateLayerTexture();
		if (p->GetCreateObject() == m_pWindow)
			m_pHardwareComposition->SetRootLayerSurface(pGpuTexture);

		m_bNeedRebuildGpuLayerTree = true;
	}

	return pGpuTexture;
}

void  WindowRender::OnRenderLayerDestroy(RenderLayer* p)
{
    if (m_pHardwareComposition)
    {
        m_bNeedRebuildGpuLayerTree = true;
    }
}   

// IRenderTarget* 没有引用计数机制
// 但仍然采用Release进行释放（delete） 
bool  WindowRender::CreateRenderTarget(IRenderTarget** pp)
{
    if (!pp)
        return false;

    *pp = UICreateRenderTarget(m_pWindow->GetIUIApplication(), m_eGRL, m_bNeedAlphaChannel);
    return true;
}

void  WindowRender::OnWindowTransparentModeChanged(IWindowTransparent* pMode)
{
    if (pMode && pMode->RequireAlphaChannel())
    {
        if (m_eGRL == GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
        {
            m_eGRL = GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
        }
    }
}

void  WindowRender::UpdateWindow(HDC hDC, RECT* prcDamage)
{
    if (m_bFirstTimePaintWindow)
    {
        m_bFirstTimePaintWindow = false;
        SetCanCommit(true);
    }

	CRect rcClient;
    if (!prcDamage)
    {
		::GetClientRect(m_pWindow->m_hWnd, &rcClient);
		prcDamage = &rcClient;
	}

	RenderLayer* pRenderLayer = m_pWindow->GetRenderLayer2();
	if (!pRenderLayer)
		return;

	pRenderLayer->UpdateLayer(prcDamage, 1);
	Commit(hDC, prcDamage, 1);
}

void WindowRender::OnWindowSize(UINT nWidth, UINT nHeight)
{
    if (m_pHardwareComposition)
        m_pHardwareComposition->Resize(nWidth, nHeight);
}

void  WindowRender::SetCanCommit(bool b)
{
    if (b)
        m_lRefCanCommit --;
    else
        m_lRefCanCommit ++;
}

bool  WindowRender::CanCommit() 
{ 
    return 0 == m_lRefCanCommit; 
}
void  WindowRender::Commit(HDC hDC, RECT* prc, int nCount)
{
    if (!CanCommit())
        return;

    if (m_pHardwareComposition)
    {
        HardComposite();
    }
    else
    {
        m_pWindow->CommitDoubleBuffet2Window(hDC, prc, prc?nCount:0);
    }
}

void  WindowRender::HardComposite()
{
    if (m_pHardwareComposition)
    {
        if (m_bNeedRebuildGpuLayerTree)
            this->RebuildCompositingLayerTree();

        if (m_pHardwareComposition->BeginCommit())
        {
            RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();

			RECT rcClip;
			::GetClientRect(m_pWindow->GetHWND(), &rcClip);

			GpuLayerCommitContext context;
			context.SetClipRect(&rcClip);
            pRootLayer->Compositor2Gpu(context);
            m_pHardwareComposition->EndCommit();
        }
    }
}

void  WindowRender::OnHardCompositeChanged(bool bEnable)
{
	if (m_bNeedRebuildGpuLayerTree)
		this->RebuildCompositingLayerTree();

	RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();
	pRootLayer->OnHardCompositeChanged(bEnable);
}

// 通过renderlayer树来构建gpulayer树
void  WindowRender::RebuildCompositingLayerTree()
{
    if (!m_bNeedRebuildGpuLayerTree)
        return;

    // 递归
//     IGpuLayerTexture* pRootLayer = m_pWindow->GetRenderLayer2()->GetGpuTexture();
//     pRootLayer->ClearChildren();

    RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();
    pRootLayer->ClearChildren();
    rebuildCompositingLayerTree(m_pWindow,pRootLayer);

    m_bNeedRebuildGpuLayerTree = false;
}
void  WindowRender::rebuildCompositingLayerTree(
        Object* pParent,
        RenderLayer* pParentLayer)
{
    if (!pParent || !pParentLayer)
        return;

    Object*  pChild = NULL;
    RenderLayer*  pLayer = NULL;

    while (pChild = pParent->EnumAllChildObject(pChild))
    {
        pLayer = pChild->GetSelfRenderLayer2();
        if (pLayer)
        {
//             IGpuLayerTexture*  pGpuLayerTexture = pLayer->GetGpuTexture();
//             UIASSERT(pGpuLayerTexture);

//             pGpuLayerTexture->ClearChildren();
//             pParentLayer->AddChild(pGpuLayerTexture);
// #ifdef _DEBUG
//             POINT pt = pChild->GetWindowPoint();
//             pGpuLayerTexture->SetWindowPos(pt.x, pt.y);
// #endif            
//             rebuildCompositingLayerTree(pChild, pGpuLayerTexture);

            pLayer->ClearChildren();
            pParentLayer->AddChild(pLayer);
            rebuildCompositingLayerTree(pChild, pLayer);
        }
        else
        {
            rebuildCompositingLayerTree(pChild, pParentLayer);
        }
    }
}

void  WindowRender::SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye)
{
	// 仅在窗口创建之前设置有用
	m_eGRL = eTpye;
}
GRAPHICS_RENDER_LIBRARY_TYPE  WindowRender::GetGraphicsRenderType()
{
	return m_eGRL;
}
bool  WindowRender::GetRequireAlphaChannel()
{
    return m_bNeedAlphaChannel;

	// return m_eGRL==GRAPHICS_RENDER_LIBRARY_TYPE_GDI?false:true;  
}

